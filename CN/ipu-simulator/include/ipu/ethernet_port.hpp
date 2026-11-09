#pragma once

#include <cstdint>
#include <string>

enum class PortAdminState {
    DOWN,
    UP
};

enum class PortOperState {
    DOWN,
    UP
};

struct PortStatistics {
    std::uint64_t rx_packets{0};
    std::uint64_t tx_packets{0};
    std::uint64_t rx_bytes{0};
    std::uint64_t tx_bytes{0};
    std::uint64_t rx_dropped{0};
    std::uint64_t tx_dropped{0};
};

struct EthernetPort {
    std::uint32_t port_id{0};
    std::string mac_address;

    std::uint32_t mtu{1500};
    std::uint32_t speed_mbps{10000};

    PortAdminState admin_state{PortAdminState::DOWN};
    PortOperState oper_state{PortOperState::DOWN};

    bool link_present{false};

    PortStatistics stats;
};
