#include "hardware/l2_forwarding/l2_forwarding.hpp"

#include <algorithm>
#include <tuple>

bool FdbKey::operator<(const FdbKey& other) const noexcept
{
    return std::tie(vlan_id, mac) <
           std::tie(other.vlan_id, other.mac);
}

L2ForwardingEngine::L2ForwardingEngine(PortManager& ports)
    : ports_(ports)
{
}

L2ForwardingEngine::L2ForwardingEngine(
    PortManager& ports,
    VlanManager& vlans)
    : ports_(ports),
      vlans_(&vlans)
{
}

bool L2ForwardingEngine::is_unicast(
    const MacAddress& mac) const noexcept
{
    return (mac[0] & 0x01U) == 0;
}

bool L2ForwardingEngine::is_port_eligible(
    std::uint32_t port_id) const noexcept
{
    const EthernetPort* port = ports_.get_port(port_id);

    return port != nullptr &&
           port->admin_state == PortAdminState::UP &&
           port->oper_state == PortOperState::UP;
}

bool L2ForwardingEngine::learn(
    std::uint16_t vlan_id,
    const MacAddress& source_mac,
    std::uint32_t ingress_port)
{
    if (!is_port_eligible(ingress_port) ||
        !is_unicast(source_mac)) {
        return false;
    }

    if (vlans_ != nullptr &&
        !vlans_->is_member(ingress_port, vlan_id)) {
        return false;
    }

    const bool all_zero = std::all_of(
        source_mac.begin(),
        source_mac.end(),
        [](std::uint8_t byte) {
            return byte == 0;
        });

    if (all_zero) {
        return false;
    }

    fdb_[FdbKey{vlan_id, source_mac}] = FdbEntry{ingress_port};
    return true;
}

bool L2ForwardingEngine::lookup(
    std::uint16_t vlan_id,
    const MacAddress& destination_mac,
    std::uint32_t& port_id) const
{
    const auto it = fdb_.find(FdbKey{vlan_id, destination_mac});

    if (it == fdb_.end()) {
        return false;
    }

    port_id = it->second.port_id;
    return true;
}

std::vector<std::uint32_t> L2ForwardingEngine::flood_ports(
    std::uint16_t vlan_id,
    std::uint32_t ingress_port) const
{
    std::vector<std::uint32_t> result;

    for (std::uint32_t id = 0; id < ports_.port_count(); ++id) {
        if (id == ingress_port || !is_port_eligible(id)) {
            continue;
        }

        if (vlans_ != nullptr &&
            !vlans_->is_member(id, vlan_id)) {
            continue;
        }

        result.push_back(id);
    }

    return result;
}

L2ForwardingDecision L2ForwardingEngine::process(
    const ParsedPacket& packet,
    std::uint32_t ingress_port)
{
    const std::uint16_t vlan_id =
        packet.ethernet.vlan_tagged
            ? packet.ethernet.vlan_id
            : 0;

    return process(packet, ingress_port, vlan_id);
}

L2ForwardingDecision L2ForwardingEngine::process(
    const ParsedPacket& packet,
    std::uint32_t ingress_port,
    std::uint16_t vlan_id)
{
    L2ForwardingDecision decision;

    if (!is_port_eligible(ingress_port)) {
        return decision;
    }

    if (vlans_ != nullptr &&
        !vlans_->is_member(ingress_port, vlan_id)) {
        return decision;
    }

    const EthernetMetadata& ethernet = packet.ethernet;

    decision.source_learned =
        learn(vlan_id, ethernet.src_mac, ingress_port);

    const MacAddress& destination = ethernet.dst_mac;

    if (!is_unicast(destination)) {
        decision.action = L2ForwardingAction::FLOOD;
        decision.egress_ports = flood_ports(vlan_id, ingress_port);
        return decision;
    }

    std::uint32_t destination_port = 0;

    if (!lookup(vlan_id, destination, destination_port) ||
        !is_port_eligible(destination_port) ||
        (vlans_ != nullptr &&
         !vlans_->is_member(destination_port, vlan_id))) {
        decision.action = L2ForwardingAction::FLOOD;
        decision.egress_ports = flood_ports(vlan_id, ingress_port);
        return decision;
    }

    if (destination_port == ingress_port) {
        decision.action = L2ForwardingAction::DROP;
        decision.drop_reason =
            L2DropReason::DESTINATION_ON_INGRESS_PORT;
        return decision;
    }

    decision.action = L2ForwardingAction::KNOWN_UNICAST;
    decision.egress_ports.push_back(destination_port);

    return decision;
}

void L2ForwardingEngine::clear() noexcept
{
    fdb_.clear();
}

std::size_t L2ForwardingEngine::fdb_size() const noexcept
{
    return fdb_.size();
}
