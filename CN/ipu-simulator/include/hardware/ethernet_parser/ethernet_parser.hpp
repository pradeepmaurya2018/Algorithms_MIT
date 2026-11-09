#pragma once

#include "hardware/ethernet_parser/ethernet.hpp"

#include <cstdint>
#include <span>

enum class EthernetParseStatus {
    OK,
    FRAME_TOO_SHORT,
    TRUNCATED_VLAN_HEADER
};

struct EthernetParseResult {
    EthernetParseStatus status{EthernetParseStatus::FRAME_TOO_SHORT};
    EthernetMetadata metadata;
};

class EthernetParser {
public:
    static EthernetParseResult parse(
        std::span<const std::uint8_t> frame
    );
};
