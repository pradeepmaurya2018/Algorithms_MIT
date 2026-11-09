#pragma once

#include <cstdint>
#include <string>

struct IpuInfo {
    std::string name;
    std::string model;
    std::string version;

    std::uint32_t port_count{0};
    std::uint32_t max_packet_size{0};
};
