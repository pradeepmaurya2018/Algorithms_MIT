#pragma once

#include "nic/nic_device.h"

#include <string>

class LinuxNic final : public NicDevice {
public:
    explicit LinuxNic(std::string pci_address);

    bool discover() override;
    const NicInfo& info() const override;

private:
    std::string pci_address_;
    NicInfo info_;

    bool read_text(const std::string& path, std::string& value) const;
    bool read_hex_u16(const std::string& path, uint16_t& value) const;
    bool read_hex_u8(const std::string& path, uint8_t& value) const;
    bool parse_resource_file();
    bool find_interface_and_driver();
    bool read_mac();
};
