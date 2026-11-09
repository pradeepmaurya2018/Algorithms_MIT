#pragma once

#include "hardware/ipv6_parser/ipv6.hpp"

#include <cstdint>
#include <span>

enum class Ipv6ParseStatus {
    OK,
    PACKET_TOO_SHORT,
    INVALID_VERSION,
    INVALID_PAYLOAD_LENGTH
};

struct Ipv6ParseResult {
    Ipv6ParseStatus status{Ipv6ParseStatus::PACKET_TOO_SHORT};
    Ipv6Metadata metadata;
};

class Ipv6Parser {
public:
    static Ipv6ParseResult parse(
        std::span<const std::uint8_t> packet);
};
