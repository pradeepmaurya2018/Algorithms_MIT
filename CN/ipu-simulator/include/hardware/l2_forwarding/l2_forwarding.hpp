#pragma once

#include "hardware/packet_parser/parsed_packet.hpp"
#include "hardware/vlan/vlan_manager.hpp"
#include "ipu/port_manager.hpp"

#include <array>
#include <cstdint>
#include <map>
#include <vector>

using MacAddress = std::array<std::uint8_t, 6>;

struct FdbKey {
    std::uint16_t vlan_id{0};
    MacAddress mac{};

    bool operator<(const FdbKey& other) const noexcept;
};

struct FdbEntry {
    std::uint32_t port_id{0};
};

enum class L2ForwardingAction {
    KNOWN_UNICAST,
    FLOOD,
    DROP
};

enum class L2DropReason {
    NONE,
    DESTINATION_ON_INGRESS_PORT
};

struct L2ForwardingDecision {
    L2ForwardingAction action{L2ForwardingAction::DROP};
    L2DropReason drop_reason{L2DropReason::NONE};
    std::vector<std::uint32_t> egress_ports;
    bool source_learned{false};
};

class L2ForwardingEngine {
public:
    explicit L2ForwardingEngine(PortManager& ports);
    L2ForwardingEngine(PortManager& ports, VlanManager& vlans);

    L2ForwardingDecision process(
        const ParsedPacket& packet,
        std::uint32_t ingress_port);

    L2ForwardingDecision process(
        const ParsedPacket& packet,
        std::uint32_t ingress_port,
        std::uint16_t vlan_id);

    bool learn(
        std::uint16_t vlan_id,
        const MacAddress& source_mac,
        std::uint32_t ingress_port);

    bool lookup(
        std::uint16_t vlan_id,
        const MacAddress& destination_mac,
        std::uint32_t& port_id) const;

    void clear() noexcept;
    std::size_t fdb_size() const noexcept;

private:
    bool is_port_eligible(std::uint32_t port_id) const noexcept;
    bool is_unicast(const MacAddress& mac) const noexcept;

    std::vector<std::uint32_t> flood_ports(
        std::uint16_t vlan_id,
        std::uint32_t ingress_port) const;

    PortManager& ports_;
    VlanManager* vlans_{nullptr};
    std::map<FdbKey, FdbEntry> fdb_;
};
