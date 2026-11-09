# IPU Simulator

A C++20 software simulator and validation framework for
networking Infrastructure Processing Units (IPUs).

## Objectives

- Model an IPU device and its lifecycle.
- Simulate Ethernet ports and packet processing.
- Implement programmable packet-processing pipelines.
- Model L2/L3 forwarding and policy engines.
- Build reusable automated validation tests.

## Technology

- C++20
- CMake
- Linux
- GoogleTest (planned)

## Build

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
./build/ipu-simulator

# 7. Build and run

```bash
cmake -S . -B build
cmake --build build -j$(nproc)
./build/ipu-simulator
cat > include/ipu/ipu_info.hpp <<'EOF'
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
