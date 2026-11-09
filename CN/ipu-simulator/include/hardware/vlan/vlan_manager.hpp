#pragma once

#include "hardware/vlan/vlan.hpp"
#include "ipu/port_manager.hpp"

#include <cstdint>
#include <map>
#include <set>
#include <vector>

class VlanManager {
public:
    explicit VlanManager(PortManager& ports);

    bool create_vlan(std::uint16_t vlan_id);
    bool delete_vlan(std::uint16_t vlan_id);

    bool configure_access_port(
        std::uint32_t port_id,
        std::uint16_t pvid);

    bool configure_trunk_port(
        std::uint32_t port_id,
        const std::vector<std::uint16_t>& allowed_vlans);

    bool is_valid_vlan(std::uint16_t vlan_id) const noexcept;

    bool is_member(
        std::uint32_t port_id,
        std::uint16_t vlan_id) const noexcept;

    bool is_access_port(std::uint32_t port_id) const noexcept;
    bool is_trunk_port(std::uint32_t port_id) const noexcept;

    std::uint16_t pvid(std::uint32_t port_id) const noexcept;

    std::vector<std::uint32_t> member_ports(
        std::uint16_t vlan_id) const;

    void reset() noexcept;

private:
    bool valid_port(std::uint32_t port_id) const noexcept;

    PortManager& ports_;
    std::set<std::uint16_t> vlans_;
    std::map<std::uint32_t, VlanPortConfig> port_configs_;
};
