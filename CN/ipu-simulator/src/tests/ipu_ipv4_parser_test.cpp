#include "tests/ipu_ipv4_parser_test.hpp"
#include "hardware/ipv4_parser/ipv4_parser.hpp"

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
    std::size_t header_length
)
{
    std::uint32_t sum = 0;

    for (std::size_t i = 0; i < header_length; i += 2) {
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
    // IPv4 header: 20 bytes.
    // Payload: 8 bytes.
    std::vector<std::uint8_t> packet(28, 0);

    packet[0] = 0x45; // Version 4, IHL 5.
    packet[1] = 0xBA; // DSCP 46, ECN 2.

    write_be16(packet, 2, 28);       // Total length.
    write_be16(packet, 4, 0x1234);   // Identification.
    write_be16(packet, 6, 0x2003);   // MF=1, fragment offset=3.

    packet[8] = 64;  // TTL.
    packet[9] = 17;  // UDP.

    // Header checksum is initially zero.

    // Source: 192.0.2.1
    packet[12] = 192;
    packet[13] = 0;
    packet[14] = 2;
    packet[15] = 1;

    // Destination: 198.51.100.2
    packet[16] = 198;
    packet[17] = 51;
    packet[18] = 100;
    packet[19] = 2;

    const std::uint16_t checksum =
        calculate_checksum(packet, 20);

    write_be16(packet, 10, checksum);

    return packet;
}

} // namespace

std::string IpuIpv4ParserTest::name() const
{
    return "IPU_IPV4_PARSER";
}

TestResult IpuIpv4ParserTest::run()
{
    const auto fail = [&](const std::string& reason) {
        return TestResult{
            TestStatus::FAIL,
            name(),
            reason
        };
    };

    // Test 1: Valid IPv4 packet.
    const auto packet = make_ipv4_packet();

    const Ipv4ParseResult result = Ipv4Parser::parse(packet);

    if (result.status != Ipv4ParseStatus::OK) {
        return fail("Valid IPv4 packet was rejected");
    }

    const Ipv4Metadata& ip = result.metadata;

    if (ip.version != 4 || ip.ihl != 5) {
        return fail("Incorrect IPv4 version or IHL");
    }

    if (ip.dscp != 46 || ip.ecn != 2) {
        return fail("Incorrect DSCP or ECN extraction");
    }

    if (ip.total_length != 28) {
        return fail("Incorrect IPv4 total length");
    }

    if (ip.identification != 0x1234) {
        return fail("Incorrect identification field");
    }

    if (!ip.more_fragments || ip.fragment_offset != 3) {
        return fail("Incorrect fragmentation metadata");
    }

    if (ip.dont_fragment || ip.reserved_flag) {
        return fail("Unexpected IPv4 fragmentation flags");
    }

    if (ip.ttl != 64 || ip.protocol != 17) {
        return fail("Incorrect TTL or protocol");
    }

    if (ip.src_ip != 0xC0000201U ||
        ip.dst_ip != 0xC6336402U) {
        return fail("Incorrect source or destination IPv4 address");
    }

    if (!ip.checksum_valid) {
        return fail("Valid IPv4 checksum was not recognized");
    }

    if (ip.header_length != 20 ||
        ip.payload_offset != 20 ||
        ip.payload_length != 8) {
        return fail("Incorrect IPv4 header or payload boundaries");
    }

    // Test 2: Invalid IPv4 version.
    auto invalid_version = packet;
    invalid_version[0] = 0x65;

    if (Ipv4Parser::parse(invalid_version).status !=
        Ipv4ParseStatus::INVALID_VERSION) {
        return fail("Invalid IP version was not rejected");
    }

    // Test 3: Invalid IHL.
    auto invalid_ihl = packet;
    invalid_ihl[0] = 0x44;

    if (Ipv4Parser::parse(invalid_ihl).status !=
        Ipv4ParseStatus::INVALID_IHL) {
        return fail("Invalid IHL was not rejected");
    }

    // Test 4: Total length smaller than header.
    auto short_total_length = packet;
    write_be16(short_total_length, 2, 19);

    if (Ipv4Parser::parse(short_total_length).status !=
        Ipv4ParseStatus::INVALID_TOTAL_LENGTH) {
        return fail("Total length smaller than header was accepted");
    }

    // Test 5: Total length exceeds available bytes.
    auto oversized_total_length = packet;
    write_be16(oversized_total_length, 2, 100);

    if (Ipv4Parser::parse(oversized_total_length).status !=
        Ipv4ParseStatus::INVALID_TOTAL_LENGTH) {
        return fail("Total length exceeding buffer was accepted");
    }

    // Test 6: Invalid header checksum.
    auto bad_checksum = packet;
    bad_checksum[10] ^= 0x01;

    if (Ipv4Parser::parse(bad_checksum).status !=
        Ipv4ParseStatus::HEADER_CHECKSUM_INVALID) {
        return fail("Invalid IPv4 checksum was not rejected");
    }

    // Test 7: Truncated IPv4 header.
    std::vector<std::uint8_t> truncated(19, 0);

    if (Ipv4Parser::parse(truncated).status !=
        Ipv4ParseStatus::PACKET_TOO_SHORT) {
        return fail("Truncated IPv4 header was not rejected");
    }

    return {
        TestStatus::PASS,
        name(),
        "IPv4 fields, fragmentation, boundaries and checksum validated"
    };
}
