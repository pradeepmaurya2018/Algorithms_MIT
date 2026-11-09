#pragma once

#include <cstdint>
#include <set>

enum class VlanPortMode {
    ACCESS,
    TRUNK
};

struct VlanPortConfig {
    VlanPortMode mode{VlanPortMode::ACCESS};
    std::uint16_t pvid{0};
    std::set<std::uint16_t> allowed_vlans;
};

enum class VlanClassifyStatus {
    OK,
    INVALID_PORT,
    TAGGED_FRAME_ON_ACCESS_PORT,
    UNTAGGED_FRAME_ON_TRUNK_PORT,
    INVALID_VLAN,
    VLAN_NOT_ALLOWED
};

struct VlanClassification {
    VlanClassifyStatus status{VlanClassifyStatus::INVALID_PORT};
    std::uint16_t vlan_id{0};

    bool ok() const noexcept
    {
        return status == VlanClassifyStatus::OK;
    }
};
