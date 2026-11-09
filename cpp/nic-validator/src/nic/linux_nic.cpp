#include "nic/linux_nic.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <optional>
#include <sstream>

namespace fs = std::filesystem;

LinuxNic::LinuxNic(std::string pci_address)
    : pci_address_(std::move(pci_address)) {
    info_.pci_address = pci_address_;
}

bool LinuxNic::read_text(const std::string& path, std::string& value) const {
    std::ifstream file(path);
    if (!file) {
        return false;
    }
    std::getline(file, value);
    return !file.bad();
}

bool LinuxNic::read_hex_u16(const std::string& path, uint16_t& value) const {
    std::string s;
    if (!read_text(path, s)) {
        return false;
    }

    try {
        value = static_cast<uint16_t>(std::stoul(s, nullptr, 0));
    } catch (...) {
        return false;
    }
    return true;
}

bool LinuxNic::read_hex_u8(const std::string& path, uint8_t& value) const {
    std::string s;
    if (!read_text(path, s)) {
        return false;
    }

    try {
        value = static_cast<uint8_t>(std::stoul(s, nullptr, 0));
    } catch (...) {
        return false;
    }
    return true;
}

bool LinuxNic::parse_resource_file() {
    const std::string path =
        "/sys/bus/pci/devices/" + pci_address_ + "/resource";

    std::ifstream file(path);
    if (!file) {
        return false;
    }

    std::string line;
    int index = 0;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string start_s, end_s, flags_s;

        if (!(iss >> start_s >> end_s >> flags_s)) {
            ++index;
            continue;
        }

        try {
            const uint64_t start = std::stoull(start_s, nullptr, 16);
            const uint64_t end   = std::stoull(end_s, nullptr, 16);
            const uint64_t flags = std::stoull(flags_s, nullptr, 16);

            if (start != 0 || end != 0) {
                PciBar bar;
                bar.index = index;
                bar.start = start;
                bar.size = (end >= start) ? (end - start + 1U) : 0U;
                bar.is_io = (flags & 0x00000100ULL) != 0;
                bar.is_mem = !bar.is_io;
                bar.prefetchable = (flags & 0x00002000ULL) != 0;
                info_.bars.push_back(bar);
            }
        } catch (...) {
            return false;
        }

        ++index;
    }

    return true;
}

bool LinuxNic::find_interface_and_driver() {
    const fs::path net_path =
        fs::path("/sys/bus/pci/devices") / pci_address_ / "net";

    if (fs::exists(net_path) && fs::is_directory(net_path)) {
        for (const auto& entry : fs::directory_iterator(net_path)) {
            info_.interface_name = entry.path().filename().string();
            break;
        }
    }

    const fs::path driver_link =
        fs::path("/sys/bus/pci/devices") / pci_address_ / "driver";

    if (fs::exists(driver_link)) {
        try {
            info_.driver = fs::read_symlink(driver_link).filename().string();
        } catch (...) {
            info_.driver.clear();
        }
    }

    return true;
}

bool LinuxNic::read_mac() {
    if (info_.interface_name.empty()) {
        return false;
    }

    const std::string path =
        "/sys/class/net/" + info_.interface_name + "/address";

    return read_text(path, info_.mac_address);
}

bool LinuxNic::discover() {
    const fs::path base =
        fs::path("/sys/bus/pci/devices") / pci_address_;

    if (!fs::exists(base)) {
        return false;
    }

    if (!read_hex_u16((base / "vendor").string(), info_.vendor_id)) {
        return false;
    }

    if (!read_hex_u16((base / "device").string(), info_.device_id)) {
        return false;
    }

    if (!read_hex_u8((base / "revision").string(), info_.revision)) {
        return false;
    }

    // class is 24 bits in sysfs. We keep the lower 16 bits for now.
    std::string class_s;
    if (read_text((base / "class").string(), class_s)) {
        try {
            const uint32_t cls = static_cast<uint32_t>(
                std::stoul(class_s, nullptr, 0));
            info_.class_code = static_cast<uint16_t>(cls & 0xffffU);
        } catch (...) {
            // Class is informational in this first testcase.
        }
    }

    if (!parse_resource_file()) {
        return false;
    }

    find_interface_and_driver();
    read_mac();

    return true;
}

const NicInfo& LinuxNic::info() const {
    return info_;
}
