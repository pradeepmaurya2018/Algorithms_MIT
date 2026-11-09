#include "hardware/packet_parser/packet_parser.hpp"

namespace {

constexpr std::uint16_t ETHERTYPE_IPV4 = 0x0800;
constexpr std::uint16_t ETHERTYPE_IPV6 = 0x86DD;

} // namespace

PacketParseResult PacketParser::parse(
    std::span<const std::uint8_t> frame)
{
    PacketParseResult result;

    const EthernetParseResult ethernet_result =
        EthernetParser::parse(frame);

    result.ethernet_status = ethernet_result.status;

    if (ethernet_result.status != EthernetParseStatus::OK) {
        result.status = PacketParseStatus::ETHERNET_ERROR;
        return result;
    }

    result.packet.ethernet = ethernet_result.metadata;

    const std::size_t offset =
        ethernet_result.metadata.payload_offset;

    const auto payload = frame.subspan(offset);

    switch (ethernet_result.metadata.ether_type) {
    case ETHERTYPE_IPV4: {
        const auto parsed = Ipv4Parser::parse(payload);
        result.ipv4_status = parsed.status;

        if (parsed.status != Ipv4ParseStatus::OK) {
            result.status = PacketParseStatus::IPV4_ERROR;
            return result;
        }

        result.packet.ipv4 = parsed.metadata;
        break;
    }

    case ETHERTYPE_IPV6: {
        const auto parsed = Ipv6Parser::parse(payload);
        result.ipv6_status = parsed.status;

        if (parsed.status != Ipv6ParseStatus::OK) {
            result.status = PacketParseStatus::IPV6_ERROR;
            return result;
        }

        result.packet.ipv6 = parsed.metadata;
        break;
    }

    default:
        // Unsupported L3 payload: retain Ethernet metadata.
        break;
    }

    result.status = PacketParseStatus::OK;
    return result;
}
