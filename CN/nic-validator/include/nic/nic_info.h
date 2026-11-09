#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct PciBar {
    int index{-1};
    uint64_t start{0};
    uint64_t size{0};
    bool is_io{false};
    bool is_mem{false};
    bool prefetchable{false};
};

struct NicInfo {
    std::string pci_address;
    uint16_t vendor_id{0};
    uint16_t device_id{0};
    uint8_t revision{0};
    uint16_t class_code{0};
    std::string interface_name;
    std::string driver;
    std::string mac_address;

    int mtu{0};
    std::string operstate;
    int carrier{-1};

    std::vector<PciBar> bars;
};
