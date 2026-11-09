#include "tests/ipu_packet_parser_test.hpp"
#include "hardware/packet_parser/packet_parser.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace {

void write_be16(
    std::vector<std::uint8_t>& data,
    std::size_t offset,
    std::uint16_t value
)
{
    data[offset] = static_cast<std::uint8_t>(value >> 8U);
    data[offset + 1] = static_cast<std::uint8_t>(value & 0xFFU);
}

std::uint16_t calculate_checksum(
    const std::vector<std::uint8_t>& data,
    std::size_t offset,
    std::size_t length
)
{
    std::uint32_t sum = 0;

    for (std::size_t i = offset; i < offset + length; i += 2) {
        const std::uint16_t word =
            static_cast<std::uint16_t>(
                (static_cast<std::uint16_t>(data[i]) << 8U) |
                static_cast<std::uint16_t>(data[i + 1])
            );

        sum += word;
    }

    while ((sum >> 16U) != 0) {
        sum = (sum & 0xFFFFU) + (sum >> 16U);
    }

    return static_cast<std::uint16_t>(~sum);
}

std::vector<std::uint8_t> make_ipv4_packet()
{
    std::vector<std::uint8_t> ip(28, 0);

    ip[0] = 0x45;
    ip[1] = 0x00;

    write_be16(ip, 2, 28);
    write_be16(ip, 4, 0x1234);
    write_be16(ip, 6, 0x4000);

    ip[8] = 64;
    ip[9] = 17;

    ip[12] = 192;
    ip[13] = 0;
    ip[14] = 2;
    ip[15] = 1;

    ip[16] = 198;
    ip[17] = 51;
    ip[18] = 100;
    ip[19] = 2;

    write_be16(ip, 10, calculate_checksum(ip, 0, 20));

    return ip;
}

std::vector<std::uint8_t> make_ethernet_frame(
    const std::vector<std::uint8_t>& payload,
    std::uint16_t ether_type
)
{
    std::vector<std::uint8_t> frame = {
        0x02, 0x00, 0x00, 0x00, 0x00, 0x02,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x01,
        static_cast<std::uint8_t>(ether_type >> 8U),
        static_cast<std::uint8_t>(ether_type & 0xFFU)
    };

    frame.insert(frame.end(), payload.begin(), payload.end());
    return frame;
}

std::vector<std::uint8_t> make_vlan_frame(
    const std::vector<std::uint8_t>& payload
)
{
    std::vector<std::uint8_t> frame = {
        0x02, 0x00, 0x00, 0x00, 0x00, 0x02,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x81, 0x00,
        0x00, 0x64,
        0x08, 0x00
    };

    frame.insert(frame.end(), payload.begin(), payload.end());
    return frame;
}

} // namespace

std::string IpuPacketParserTest::name() const
{
    return "IPU_PACKET_PARSER";
}

TestResult IpuPacketParserTest::run()
{
    const auto fail = [&](const std::string& reason) {
        return TestResult{
            TestStatus::FAIL,
            name(),
            reason
        };
    };

    const auto ipv4 = make_ipv4_packet();

    // Test 1: Untagged Ethernet + IPv4.
    const auto ethernet_ipv4 =
        make_ethernet_frame(ipv4, 0x0800);

    const PacketParseResult basic =
        PacketParser::parse(ethernet_ipv4);

    if (basic.status != PacketParseStatus::OK) {
        return fail("Ethernet + IPv4 packet was rejected");
    }

    if (!basic.packet.is_ipv4()) {
        return fail("IPv4 metadata was not populated");
    }

    if (basic.packet.ethernet.ether_type != 0x0800 ||
        basic.packet.ethernet.payload_offset != 14) {
        return fail("Incorrect Ethernet metadata");
    }

    if (basic.packet.ipv4->src_ip != 0xC0000201U ||
        basic.packet.ipv4->dst_ip != 0xC6336402U) {
        return fail("Incorrect IPv4 addresses after protocol dispatch");
    }

    // Test 2: VLAN + IPv4.
    const auto vlan_ipv4 = make_vlan_frame(ipv4);

    const PacketParseResult vlan =
        PacketParser::parse(vlan_ipv4);

    if (vlan.status != PacketParseStatus::OK) {
        return fail("VLAN + IPv4 packet was rejected");
    }

    if (!vlan.packet.ethernet.vlan_tagged ||
        vlan.packet.ethernet.vlan_id != 100) {
        return fail("VLAN metadata was not retained");
    }

    if (!vlan.packet.is_ipv4() ||
        vlan.packet.ipv4->src_ip != 0xC0000201U) {
        return fail("IPv4 parsing failed after VLAN header");
    }

    if (vlan.packet.ethernet.payload_offset != 18) {
        return fail("Incorrect VLAN payload offset");
    }

    // Test 3: ARP frame — L2 parse succeeds, no IPv4 parser.
    const std::vector<std::uint8_t> arp_payload(28, 0);

    const auto arp_frame =
        make_ethernet_frame(arp_payload, 0x0806);

    const PacketParseResult arp =
        PacketParser::parse(arp_frame);

    if (arp.status != PacketParseStatus::OK) {
        return fail("ARP Ethernet frame was rejected");
    }

    if (arp.packet.is_ipv4()) {
        return fail("IPv4 metadata unexpectedly populated for ARP");
    }

    if (arp.packet.ethernet.ether_type != 0x0806) {
        return fail("ARP EtherType was not retained");
    }

    // Test 4: Ethernet is valid, but IPv4 header is malformed.
    std::vector<std::uint8_t> malformed_ip(20, 0);
    malformed_ip[0] = 0x65;

    const auto malformed_frame =
        make_ethernet_frame(malformed_ip, 0x0800);

    const PacketParseResult malformed =
        PacketParser::parse(malformed_frame);

    if (malformed.status != PacketParseStatus::IPV4_ERROR) {
        return fail("Malformed IPv4 packet was not rejected");
    }

    if (malformed.ipv4_status !=
        Ipv4ParseStatus::INVALID_VERSION) {
        return fail("Incorrect IPv4 parser error propagation");
    }

    return {
        TestStatus::PASS,
        name(),
        "Ethernet-to-IPv4 dispatch, VLAN, ARP and error propagation validated"
    };
}
