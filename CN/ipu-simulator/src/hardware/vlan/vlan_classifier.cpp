#include "hardware/vlan/vlan_classifier.hpp"

VlanClassifier::VlanClassifier(const VlanManager& manager)
    : manager_(manager)
{
}

VlanClassification VlanClassifier::classify(
    const EthernetMetadata& ethernet,
    std::uint32_t ingress_port) const noexcept
{
    if (manager_.is_access_port(ingress_port)) {
        if (ethernet.vlan_tagged) {
            return {
                VlanClassifyStatus::TAGGED_FRAME_ON_ACCESS_PORT,
                0
            };
        }

        return {
            VlanClassifyStatus::OK,
            manager_.pvid(ingress_port)
        };
    }

    if (!manager_.is_trunk_port(ingress_port)) {
        return {
            VlanClassifyStatus::INVALID_PORT,
            0
        };
    }

    if (!ethernet.vlan_tagged) {
        return {
            VlanClassifyStatus::UNTAGGED_FRAME_ON_TRUNK_PORT,
            0
        };
    }

    if (ethernet.vlan_id == 0 || ethernet.vlan_id > 4094) {
        return {
            VlanClassifyStatus::INVALID_VLAN,
            ethernet.vlan_id
        };
    }

    if (!manager_.is_member(ingress_port, ethernet.vlan_id)) {
        return {
            VlanClassifyStatus::VLAN_NOT_ALLOWED,
            ethernet.vlan_id
        };
    }

    return {
        VlanClassifyStatus::OK,
        ethernet.vlan_id
    };
}
