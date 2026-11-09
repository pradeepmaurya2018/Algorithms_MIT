#pragma once

#include "hardware/ethernet_parser/ethernet.hpp"
#include "hardware/vlan/vlan_manager.hpp"

#include <cstdint>

class VlanClassifier {
public:
    explicit VlanClassifier(const VlanManager& manager);

    VlanClassification classify(
        const EthernetMetadata& ethernet,
        std::uint32_t ingress_port) const noexcept;

private:
    const VlanManager& manager_;
};
