#pragma once

#include "hardware/ethernet_parser/ethernet.hpp"
#include "hardware/ipv4_parser/ipv4.hpp"
#include "hardware/ipv6_parser/ipv6.hpp"

#include <optional>

struct ParsedPacket {
    EthernetMetadata ethernet;
    std::optional<Ipv4Metadata> ipv4;
    std::optional<Ipv6Metadata> ipv6;

    bool is_ipv4() const noexcept {
        return ipv4.has_value();
    }

    bool is_ipv6() const noexcept {
        return ipv6.has_value();
    }
};
