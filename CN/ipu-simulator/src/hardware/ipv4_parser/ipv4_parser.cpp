#include "hardware/ipv4_parser/ipv4_parser.hpp"

namespace {

constexpr std::size_t IPV4_MIN_HEADER_SIZE = 20;
constexpr std::uint8_t IPV4_VERSION = 4;

std::uint16_t read_be16(
    std::span<const std::uint8_t> data,
    std::size_t offset
)
{
    return static_cast<std::uint16_t>(
        (static_cast<std::uint16_t>(data[offset]) << 8U) |
        static_cast<std::uint16_t>(data[offset + 1])
    );
}

std::uint32_t read_be32(
    std::span<const std::uint8_t> data,
    std::size_t offset
)
{
    return
        (static_cast<std::uint32_t>(data[offset]) << 24U) |
        (static_cast<std::uint32_t>(data[offset + 1]) << 16U) |
        (static_cast<std::uint32_t>(data[offset + 2]) << 8U) |
        static_cast<std::uint32_t>(data[offset + 3]);
}

bool validate_checksum(
    std::span<const std::uint8_t> header
)
{
    std::uint32_t sum = 0;

    for (std::size_t i = 0; i < header.size(); i += 2) {
        sum += read_be16(header, i);
    }

    while ((sum >> 16U) != 0) {
        sum = (sum & 0xFFFFU) + (sum >> 16U);
    }

    return static_cast<std::uint16_t>(sum) == 0xFFFFU;
}

} // namespace

Ipv4ParseResult Ipv4Parser::parse(
    std::span<const std::uint8_t> packet
)
{
    Ipv4ParseResult result;

    if (packet.size() < IPV4_MIN_HEADER_SIZE) {
        result.status = Ipv4ParseStatus::PACKET_TOO_SHORT;
        return result;
    }

    Ipv4Metadata& metadata = result.metadata;

    // Version and Internet Header Length.
    metadata.version = static_cast<std::uint8_t>(
        packet[0] >> 4U
    );

    metadata.ihl = static_cast<std::uint8_t>(
        packet[0] & 0x0FU
    );

    if (metadata.version != IPV4_VERSION) {
        result.status = Ipv4ParseStatus::INVALID_VERSION;
        return result;
    }

    if (metadata.ihl < 5) {
        result.status = Ipv4ParseStatus::INVALID_IHL;
        return result;
    }

    metadata.header_length =
        static_cast<std::size_t>(metadata.ihl) * 4U;

    if (packet.size() < metadata.header_length) {
        result.status = Ipv4ParseStatus::PACKET_TOO_SHORT;
        return result;
    }

    // DSCP and ECN.
    metadata.dscp = static_cast<std::uint8_t>(
        packet[1] >> 2U
    );

    metadata.ecn = static_cast<std::uint8_t>(
        packet[1] & 0x03U
    );

    metadata.total_length = read_be16(packet, 2);
    metadata.identification = read_be16(packet, 4);

    const std::uint16_t flags_fragment = read_be16(packet, 6);

    metadata.reserved_flag =
        (flags_fragment & 0x8000U) != 0;

    metadata.dont_fragment =
        (flags_fragment & 0x4000U) != 0;

    metadata.more_fragments =
        (flags_fragment & 0x2000U) != 0;

    metadata.fragment_offset = static_cast<std::uint16_t>(
        flags_fragment & 0x1FFFU
    );

    metadata.ttl = packet[8];
    metadata.protocol = packet[9];

    metadata.header_checksum = read_be16(packet, 10);

    metadata.src_ip = read_be32(packet, 12);
    metadata.dst_ip = read_be32(packet, 16);

    // Total length must include the complete IPv4 header.
    if (metadata.total_length < metadata.header_length ||
        metadata.total_length > packet.size()) {
        result.status = Ipv4ParseStatus::INVALID_TOTAL_LENGTH;
        return result;
    }

    // Validate the checksum across the complete IPv4 header,
    // including any options.
    metadata.checksum_valid = validate_checksum(
        packet.first(metadata.header_length)
    );

    if (!metadata.checksum_valid) {
        result.status =
            Ipv4ParseStatus::HEADER_CHECKSUM_INVALID;
        return result;
    }

    metadata.payload_offset = metadata.header_length;
    metadata.payload_length =
        metadata.total_length - metadata.header_length;

    result.status = Ipv4ParseStatus::OK;
    return result;
}
