#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

struct EthernetMetadata {
    std::array<std::uint8_t, 6> dst_mac{};
    std::array<std::uint8_t, 6> src_mac{};

    std::uint16_t ether_type{0};

    bool vlan_tagged{false};

    std::uint16_t vlan_tpid{0};
    std::uint16_t vlan_id{0};

    std::uint8_t vlan_pcp{0};
    bool vlan_dei{false};

    std::size_t payload_offset{0};
};
