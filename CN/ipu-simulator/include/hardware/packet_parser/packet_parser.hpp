#pragma once

#include "hardware/packet_parser/parsed_packet.hpp"
#include "hardware/ethernet_parser/ethernet_parser.hpp"
#include "hardware/ipv4_parser/ipv4_parser.hpp"
#include "hardware/ipv6_parser/ipv6_parser.hpp"

#include <cstdint>
#include <span>

enum class PacketParseStatus {
    OK,
    ETHERNET_ERROR,
    IPV4_ERROR,
    IPV6_ERROR
};

struct PacketParseResult {
    PacketParseStatus status{PacketParseStatus::ETHERNET_ERROR};
    EthernetParseStatus ethernet_status{EthernetParseStatus::FRAME_TOO_SHORT};
    Ipv4ParseStatus ipv4_status{Ipv4ParseStatus::PACKET_TOO_SHORT};
    Ipv6ParseStatus ipv6_status{Ipv6ParseStatus::PACKET_TOO_SHORT};
    ParsedPacket packet;
};

class PacketParser {
public:
    static PacketParseResult parse(
        std::span<const std::uint8_t> frame);
};
