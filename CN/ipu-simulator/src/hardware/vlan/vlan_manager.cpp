#include "hardware/vlan/vlan_manager.hpp"

#include <algorithm>
#include <utility>

VlanManager::VlanManager(PortManager& ports)
    : ports_(ports)
{
    // VLAN 0 is retained as the simulator's legacy untagged domain.
    vlans_.insert(0);
}

bool VlanManager::valid_port(std::uint32_t port_id) const noexcept
{
    return ports_.get_port(port_id) != nullptr;
}

bool VlanManager::create_vlan(std::uint16_t vlan_id)
{
    if (vlan_id == 0 || vlan_id > 4094) {
        return false;
    }

    return vlans_.insert(vlan_id).second;
}

bool VlanManager::delete_vlan(std::uint16_t vlan_id)
{
    if (vlan_id == 0 || !is_valid_vlan(vlan_id)) {
        return false;
    }

    for (const auto& [port_id, config] : port_configs_) {
        (void)port_id;

        if (config.mode == VlanPortMode::ACCESS &&
            config.pvid == vlan_id) {
            return false;
        }

        if (config.mode == VlanPortMode::TRUNK &&
            config.allowed_vlans.contains(vlan_id)) {
            return false;
        }
    }

    return vlans_.erase(vlan_id) != 0;
}

bool VlanManager::configure_access_port(
    std::uint32_t port_id,
    std::uint16_t pvid_value)
{
    if (!valid_port(port_id) || !is_valid_vlan(pvid_value)) {
        return false;
    }

    VlanPortConfig config;
    config.mode = VlanPortMode::ACCESS;
    config.pvid = pvid_value;

    port_configs_[port_id] = std::move(config);
    return true;
}

bool VlanManager::configure_trunk_port(
    std::uint32_t port_id,
    const std::vector<std::uint16_t>& allowed_vlans)
{
    if (!valid_port(port_id) || allowed_vlans.empty()) {
        return false;
    }

    VlanPortConfig config;
    config.mode = VlanPortMode::TRUNK;

    for (const std::uint16_t vlan_id : allowed_vlans) {
        if (vlan_id == 0 ||
            vlan_id > 4094 ||
            !is_valid_vlan(vlan_id)) {
            return false;
        }

        config.allowed_vlans.insert(vlan_id);
    }

    port_configs_[port_id] = std::move(config);
    return true;
}

bool VlanManager::is_valid_vlan(
    std::uint16_t vlan_id) const noexcept
{
    return vlans_.contains(vlan_id);
}

bool VlanManager::is_member(
    std::uint32_t port_id,
    std::uint16_t vlan_id) const noexcept
{
    if (!valid_port(port_id) || !is_valid_vlan(vlan_id)) {
        return false;
    }

    const auto it = port_configs_.find(port_id);

    // Unconfigured ports retain legacy untagged VLAN-0 behavior.
    if (it == port_configs_.end()) {
        return vlan_id == 0;
    }

    const VlanPortConfig& config = it->second;

    if (config.mode == VlanPortMode::ACCESS) {
        return config.pvid == vlan_id;
    }

    return config.allowed_vlans.contains(vlan_id);
}

bool VlanManager::is_access_port(
    std::uint32_t port_id) const noexcept
{
    if (!valid_port(port_id)) {
        return false;
    }

    const auto it = port_configs_.find(port_id);

    return it == port_configs_.end() ||
           it->second.mode == VlanPortMode::ACCESS;
}

bool VlanManager::is_trunk_port(
    std::uint32_t port_id) const noexcept
{
    const auto it = port_configs_.find(port_id);

    return valid_port(port_id) &&
           it != port_configs_.end() &&
           it->second.mode == VlanPortMode::TRUNK;
}

std::uint16_t VlanManager::pvid(
    std::uint32_t port_id) const noexcept
{
    const auto it = port_configs_.find(port_id);

    if (!valid_port(port_id) || it == port_configs_.end()) {
        return 0;
    }

    return it->second.pvid;
}

std::vector<std::uint32_t> VlanManager::member_ports(
    std::uint16_t vlan_id) const
{
    std::vector<std::uint32_t> result;

    if (!is_valid_vlan(vlan_id)) {
        return result;
    }

    for (std::uint32_t id = 0; id < ports_.port_count(); ++id) {
        if (is_member(id, vlan_id)) {
            result.push_back(id);
        }
    }

    return result;
}

void VlanManager::reset() noexcept
{
    port_configs_.clear();
    vlans_.clear();
    vlans_.insert(0);
}
