#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>

struct Packet {
    std::vector<std::uint8_t> data;

    std::uint32_t ingress_port{0};
    std::uint32_t egress_port{0};

    std::uint64_t packet_id{0};

    std::size_t size() const noexcept
    {
        return data.size();
    }

    bool empty() const noexcept
    {
        return data.empty();
    }
};
