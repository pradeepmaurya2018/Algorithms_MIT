#include "hardware/ipv6_parser/ipv6_parser.hpp"

namespace {

std::uint16_t read_be16(
    std::span<const std::uint8_t> data,
    std::size_t offset)
{
    return static_cast<std::uint16_t>(
        (static_cast<std::uint16_t>(data[offset]) << 8) |
        static_cast<std::uint16_t>(data[offset + 1]));
}

} // namespace

Ipv6ParseResult Ipv6Parser::parse(
    std::span<const std::uint8_t> packet)
{
    Ipv6ParseResult result;

    constexpr std::size_t IPV6_HEADER_SIZE = 40;

    if (packet.size() < IPV6_HEADER_SIZE) {
        result.status = Ipv6ParseStatus::PACKET_TOO_SHORT;
        return result;
    }

    const std::uint32_t first_word =
        (static_cast<std::uint32_t>(packet[0]) << 24) |
        (static_cast<std::uint32_t>(packet[1]) << 16) |
        (static_cast<std::uint32_t>(packet[2]) << 8) |
        static_cast<std::uint32_t>(packet[3]);

    const std::uint8_t version =
        static_cast<std::uint8_t>((first_word >> 28) & 0x0F);

    if (version != 6) {
        result.status = Ipv6ParseStatus::INVALID_VERSION;
        return result;
    }

    auto& metadata = result.metadata;

    metadata.version = version;
    metadata.traffic_class =
        static_cast<std::uint8_t>((first_word >> 20) & 0xFF);
    metadata.flow_label = first_word & 0x000FFFFF;

    metadata.payload_length = read_be16(packet, 4);
    metadata.next_header = packet[6];
    metadata.hop_limit = packet[7];

    if (metadata.payload_length > packet.size() - IPV6_HEADER_SIZE) {
        result.status = Ipv6ParseStatus::INVALID_PAYLOAD_LENGTH;
        return result;
    }

    for (std::size_t i = 0; i < 16; ++i) {
        metadata.src_ip[i] = packet[8 + i];
        metadata.dst_ip[i] = packet[24 + i];
    }

    metadata.header_length = IPV6_HEADER_SIZE;
    metadata.payload_offset = IPV6_HEADER_SIZE;

    result.status = Ipv6ParseStatus::OK;
    return result;
}
