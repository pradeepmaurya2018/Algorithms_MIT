#include "tests/ipu_ipv6_parser_test.hpp"

#include "hardware/ipv6_parser/ipv6_parser.hpp"
#include "hardware/packet_parser/packet_parser.hpp"

#include <cstdint>
#include <vector>

namespace {

std::vector<std::uint8_t> make_ipv6_packet()
{
    std::vector<std::uint8_t> packet(48, 0);

    // Version = 6, Traffic Class = 0x2E, Flow Label = 0x12345.
    packet[0] = 0x62;
    packet[1] = 0xE1;
    packet[2] = 0x23;
    packet[3] = 0x45;

    // Payload length = 8.
    packet[4] = 0x00;
    packet[5] = 0x08;

    // Next Header = UDP (17).
    packet[6] = 17;

    // Hop Limit = 64.
    packet[7] = 64;

    // Source: 2001:db8::1
    packet[8] = 0x20;
    packet[9] = 0x01;
    packet[10] = 0x0D;
    packet[11] = 0xB8;
    packet[23] = 0x01;

    // Destination: 2001:db8::2
    packet[24] = 0x20;
    packet[25] = 0x01;
    packet[26] = 0x0D;
    packet[27] = 0xB8;
    packet[39] = 0x02;

    return packet;
}

std::vector<std::uint8_t> make_ethernet_ipv6_frame()
{
    const auto ipv6 = make_ipv6_packet();

    std::vector<std::uint8_t> frame(14 + ipv6.size(), 0);

    // Destination MAC.
    frame[0] = 0x02;
    frame[5] = 0x02;

    // Source MAC.
    frame[6] = 0x02;
    frame[11] = 0x01;

    // EtherType IPv6.
    frame[12] = 0x86;
    frame[13] = 0xDD;

    for (std::size_t i = 0; i < ipv6.size(); ++i) {
        frame[14 + i] = ipv6[i];
    }

    return frame;
}

} // namespace

std::string IpuIpv6ParserTest::name() const
{
    return "IPU_IPV6_PARSER";
}

TestResult IpuIpv6ParserTest::run()
{
    const auto packet = make_ipv6_packet();

    const auto valid = Ipv6Parser::parse(packet);

    if (valid.status != Ipv6ParseStatus::OK ||
        valid.metadata.version != 6 ||
        valid.metadata.traffic_class != 0x2E ||
        valid.metadata.flow_label != 0x12345 ||
        valid.metadata.payload_length != 8 ||
        valid.metadata.next_header != 17 ||
        valid.metadata.hop_limit != 64 ||
        valid.metadata.payload_offset != 40 ||
        valid.metadata.src_ip[15] != 1 ||
        valid.metadata.dst_ip[15] != 2) {
        return {TestStatus::FAIL, name(),
                "IPv6 base-header parsing failed"};
    }

    auto invalid_version = packet;
    invalid_version[0] = 0x42;

    const auto bad_version = Ipv6Parser::parse(invalid_version);

    if (bad_version.status != Ipv6ParseStatus::INVALID_VERSION) {
        return {TestStatus::FAIL, name(),
                "Invalid IPv6 version was accepted"};
    }

    std::vector<std::uint8_t> short_packet(39, 0);

    const auto truncated = Ipv6Parser::parse(short_packet);

    if (truncated.status != Ipv6ParseStatus::PACKET_TOO_SHORT) {
        return {TestStatus::FAIL, name(),
                "Truncated IPv6 header was accepted"};
    }

    auto invalid_length = packet;
    invalid_length[4] = 0x00;
    invalid_length[5] = 0x20;

    const auto bad_length = Ipv6Parser::parse(invalid_length);

    if (bad_length.status != Ipv6ParseStatus::INVALID_PAYLOAD_LENGTH) {
        return {TestStatus::FAIL, name(),
                "Invalid IPv6 payload length was accepted"};
    }

    const auto frame = make_ethernet_ipv6_frame();
    const auto dispatched = PacketParser::parse(frame);

    if (dispatched.status != PacketParseStatus::OK ||
        !dispatched.packet.is_ipv6() ||
        dispatched.packet.is_ipv4()) {
        return {TestStatus::FAIL, name(),
                "Ethernet-to-IPv6 dispatch failed"};
    }

    return {TestStatus::PASS, name(),
            "IPv6 base header, validation and Ethernet dispatch validated"};
}
