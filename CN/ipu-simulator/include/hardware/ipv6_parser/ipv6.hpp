#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

struct Ipv6Metadata {
    std::uint8_t version{0};
    std::uint8_t traffic_class{0};
    std::uint32_t flow_label{0};

    std::uint16_t payload_length{0};
    std::uint8_t next_header{0};
    std::uint8_t hop_limit{0};

    std::array<std::uint8_t, 16> src_ip{};
    std::array<std::uint8_t, 16> dst_ip{};

    std::size_t header_length{40};
    std::size_t payload_offset{40};
};
