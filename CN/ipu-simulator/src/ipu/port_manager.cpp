#include "ipu/port_manager.hpp"

#include <iomanip>
#include <sstream>

bool PortManager::initialize(std::uint32_t port_count)
{
    if (!ports_.empty() || port_count == 0) {
        return false;
    }

    ports_.reserve(port_count);

    for (std::uint32_t id = 0; id < port_count; ++id) {
        EthernetPort port;

        port.port_id = id;
        port.mac_address =
            "02:00:00:00:00:";

        std::ostringstream suffix;
        suffix << std::hex
               << std::setfill('0')
               << std::setw(2)
               << (id & 0xFFU);

        port.mac_address += suffix.str();

        port.mtu = 1500;
        port.speed_mbps = 10000;
        port.admin_state = PortAdminState::DOWN;
        port.oper_state = PortOperState::DOWN;
        port.link_present = false;

        ports_.push_back(port);
    }

    return true;
}

std::uint32_t PortManager::port_count() const noexcept
{
    return static_cast<std::uint32_t>(ports_.size());
}

EthernetPort* PortManager::get_port(std::uint32_t port_id) noexcept
{
    if (port_id >= ports_.size()) {
        return nullptr;
    }

    return &ports_[port_id];
}

const EthernetPort* PortManager::get_port(
    std::uint32_t port_id
) const noexcept
{
    if (port_id >= ports_.size()) {
        return nullptr;
    }

    return &ports_[port_id];
}

void PortManager::update_oper_state(EthernetPort& port) noexcept
{
    port.oper_state =
        (port.admin_state == PortAdminState::UP && port.link_present)
        ? PortOperState::UP
        : PortOperState::DOWN;
}

bool PortManager::set_admin_state(
    std::uint32_t port_id,
    PortAdminState state
)
{
    EthernetPort* port = get_port(port_id);

    if (port == nullptr) {
        return false;
    }

    port->admin_state = state;
    update_oper_state(*port);

    return true;
}

bool PortManager::set_link_present(
    std::uint32_t port_id,
    bool present
)
{
    EthernetPort* port = get_port(port_id);

    if (port == nullptr) {
        return false;
    }

    port->link_present = present;
    update_oper_state(*port);

    return true;
}

bool PortManager::set_mtu(
    std::uint32_t port_id,
    std::uint32_t mtu
)
{
    EthernetPort* port = get_port(port_id);

    if (port == nullptr || mtu < 68 || mtu > 65535) {
        return false;
    }

    port->mtu = mtu;
    return true;
}

void PortManager::reset() noexcept
{
    ports_.clear();
}
