#pragma once

#include "hardware/ipv4_parser/ipv4.hpp"

#include <cstdint>
#include <span>

enum class Ipv4ParseStatus {
    OK,
    PACKET_TOO_SHORT,
    INVALID_VERSION,
    INVALID_IHL,
    INVALID_TOTAL_LENGTH,
    HEADER_CHECKSUM_INVALID
};

struct Ipv4ParseResult {
    Ipv4ParseStatus status{Ipv4ParseStatus::PACKET_TOO_SHORT};
    Ipv4Metadata metadata;
};

class Ipv4Parser {
public:
    static Ipv4ParseResult parse(
        std::span<const std::uint8_t> packet
    );
};
