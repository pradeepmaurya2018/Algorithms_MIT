#include "tests/ipu_ethernet_parser_test.hpp"
#include "hardware/ethernet_parser/ethernet_parser.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace {

std::vector<std::uint8_t> make_untagged_frame()
{
    std::vector<std::uint8_t> frame = {
        0x02, 0x00, 0x00, 0x00, 0x00, 0x02,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x08, 0x00
    };

    frame.resize(64, 0);
    return frame;
}

std::vector<std::uint8_t> make_vlan_frame()
{
    std::vector<std::uint8_t> frame = {
        0x02, 0x00, 0x00, 0x00, 0x00, 0x02,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x01,

        0x81, 0x00, // TPID: 802.1Q

        0xB0, 0x64, // PCP=5, DEI=1, VID=100

        0x08, 0x00  // Inner EtherType: IPv4
    };

    frame.resize(64, 0);
    return frame;
}

} // namespace

std::string IpuEthernetParserTest::name() const
{
    return "IPU_ETH_PARSER";
}

TestResult IpuEthernetParserTest::run()
{
    const auto fail = [&](const std::string& reason) {
        return TestResult{
            TestStatus::FAIL,
            name(),
            reason
        };
    };

    // Test 1: Valid untagged Ethernet frame.
    const auto untagged = make_untagged_frame();

    const EthernetParseResult basic_result =
        EthernetParser::parse(untagged);

    if (basic_result.status != EthernetParseStatus::OK) {
        return fail("Valid untagged Ethernet frame was rejected");
    }

    if (basic_result.metadata.dst_mac[5] != 0x02 ||
        basic_result.metadata.src_mac[5] != 0x01) {
        return fail("Incorrect source or destination MAC parsing");
    }

    if (basic_result.metadata.ether_type != 0x0800) {
        return fail("Incorrect EtherType for untagged frame");
    }

    if (basic_result.metadata.vlan_tagged) {
        return fail("Untagged frame incorrectly identified as VLAN");
    }

    if (basic_result.metadata.payload_offset != 14) {
        return fail("Incorrect untagged payload offset");
    }

    // Test 2: Valid 802.1Q VLAN frame.
    const auto vlan_frame = make_vlan_frame();

    const EthernetParseResult vlan_result =
        EthernetParser::parse(vlan_frame);

    if (vlan_result.status != EthernetParseStatus::OK) {
        return fail("Valid VLAN frame was rejected");
    }

    const EthernetMetadata& vlan = vlan_result.metadata;

    if (!vlan.vlan_tagged) {
        return fail("VLAN frame was not identified as tagged");
    }

    if (vlan.vlan_tpid != 0x8100) {
        return fail("Incorrect VLAN TPID");
    }

    if (vlan.vlan_id != 100) {
        return fail("Incorrect VLAN ID");
    }

    if (vlan.vlan_pcp != 5) {
        return fail("Incorrect VLAN PCP");
    }

    if (!vlan.vlan_dei) {
        return fail("Incorrect VLAN DEI");
    }

    if (vlan.ether_type != 0x0800) {
        return fail("Incorrect inner EtherType");
    }

    if (vlan.payload_offset != 18) {
        return fail("Incorrect VLAN payload offset");
    }

    // Test 3: Ethernet header shorter than 14 bytes.
    std::vector<std::uint8_t> short_frame(13, 0);

    const EthernetParseResult short_result =
        EthernetParser::parse(short_frame);

    if (short_result.status != EthernetParseStatus::FRAME_TOO_SHORT) {
        return fail("Short Ethernet frame was not rejected correctly");
    }

    // Test 4: VLAN TPID present but tag is truncated.
    std::vector<std::uint8_t> truncated_vlan(14, 0);

    truncated_vlan[12] = 0x81;
    truncated_vlan[13] = 0x00;

    const EthernetParseResult truncated_result =
        EthernetParser::parse(truncated_vlan);

    if (truncated_result.status !=
        EthernetParseStatus::TRUNCATED_VLAN_HEADER) {
        return fail("Truncated VLAN header was not rejected correctly");
    }

    return {
        TestStatus::PASS,
        name(),
        "Ethernet, VLAN, metadata extraction and malformed frames validated"
    };
}
