#include "hardware/ethernet_parser/ethernet_parser.hpp"

namespace {

constexpr std::size_t ETH_HEADER_SIZE = 14;
constexpr std::size_t VLAN_HEADER_SIZE = 18;

constexpr std::uint16_t ETHERTYPE_VLAN = 0x8100;

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

} // namespace

EthernetParseResult EthernetParser::parse(
    std::span<const std::uint8_t> frame
)
{
    EthernetParseResult result;

    if (frame.size() < ETH_HEADER_SIZE) {
        result.status = EthernetParseStatus::FRAME_TOO_SHORT;
        return result;
    }

    EthernetMetadata& metadata = result.metadata;

    // Destination and source MAC addresses.
    for (std::size_t i = 0; i < 6; ++i) {
        metadata.dst_mac[i] = frame[i];
        metadata.src_mac[i] = frame[i + 6];
    }

    // Ethernet type or VLAN TPID.
    const std::uint16_t outer_type = read_be16(frame, 12);

    if (outer_type == ETHERTYPE_VLAN) {
        if (frame.size() < VLAN_HEADER_SIZE) {
            result.status =
                EthernetParseStatus::TRUNCATED_VLAN_HEADER;
            return result;
        }

        metadata.vlan_tagged = true;
        metadata.vlan_tpid = outer_type;

        const std::uint16_t tci = read_be16(frame, 14);

        metadata.vlan_pcp = static_cast<std::uint8_t>(
            (tci >> 13U) & 0x07U
        );

        metadata.vlan_dei = ((tci >> 12U) & 0x01U) != 0;

        metadata.vlan_id = static_cast<std::uint16_t>(
            tci & 0x0FFFU
        );

        metadata.ether_type = read_be16(frame, 16);
        metadata.payload_offset = VLAN_HEADER_SIZE;
    } else {
        metadata.ether_type = outer_type;
        metadata.payload_offset = ETH_HEADER_SIZE;
    }

    result.status = EthernetParseStatus::OK;
    return result;
}
