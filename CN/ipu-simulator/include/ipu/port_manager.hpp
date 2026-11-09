#pragma once

#include "ipu/ethernet_port.hpp"

#include <cstdint>
#include <optional>
#include <vector>

class PortManager {
public:
    bool initialize(std::uint32_t port_count);

    std::uint32_t port_count() const noexcept;

    EthernetPort* get_port(std::uint32_t port_id) noexcept;
    const EthernetPort* get_port(std::uint32_t port_id) const noexcept;

    bool set_admin_state(
        std::uint32_t port_id,
        PortAdminState state
    );

    bool set_link_present(
        std::uint32_t port_id,
        bool present
    );

    bool set_mtu(
        std::uint32_t port_id,
        std::uint32_t mtu
    );

    void reset() noexcept;

private:
    std::vector<EthernetPort> ports_;

    static void update_oper_state(EthernetPort& port) noexcept;
};
