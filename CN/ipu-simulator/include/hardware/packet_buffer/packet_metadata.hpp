#pragma once

#include <cstdint>

struct PacketMetadata {
    std::uint64_t packet_id{0};
    std::uint32_t ingress_port{0};
    std::uint32_t egress_port{0};
    std::uint16_t vlan_id{0};
    std::uint16_t ether_type{0};
    std::uint8_t traffic_class{0};
    bool is_ipv4{false};
    bool is_ipv6{false};
    bool is_fragmented{false};
};

