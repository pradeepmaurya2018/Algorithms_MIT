#!/usr/bin/env bash
set -euo pipefail

ROOT="$HOME/workspaces/ipu-simulator"
cd "$ROOT"

echo "======================================"
echo " IPU-008: L2 Forwarding Implementation"
echo "======================================"

# --------------------------------------------------
# 1. Create hardware component directories
# --------------------------------------------------

mkdir -p include/hardware/{packet_io,ethernet_mac,ethernet_parser,ipv4_parser,ipv6_parser,packet_parser,l2_forwarding}

mkdir -p src/hardware/{packet_io,ethernet_mac,ethernet_parser,ipv4_parser,ipv6_parser,packet_parser,l2_forwarding}

mkdir -p tests/hardware/l2_forwarding

# --------------------------------------------------
# 2. Migrate existing hardware components
# --------------------------------------------------

move_file() {
    local source="$1"
    local destination="$2"

    if [[ -f "$source" && ! -f "$destination" ]]; then
        mv "$source" "$destination"
    elif [[ -f "$source" && -f "$destination" ]]; then
        echo "ERROR: Both source and destination exist:"
        echo "  $source"
        echo "  $destination"
        exit 1
    fi
}

move_file include/datapath/packet.hpp \
          include/hardware/packet_io/packet.hpp

move_file include/datapath/packet_io.hpp \
          include/hardware/packet_io/packet_io.hpp

move_file src/datapath/packet_io.cpp \
          src/hardware/packet_io/packet_io.cpp

move_file include/datapath/ethernet.hpp \
          include/hardware/ethernet_parser/ethernet.hpp

move_file include/datapath/ethernet_parser.hpp \
          include/hardware/ethernet_parser/ethernet_parser.hpp

move_file src/datapath/ethernet_parser.cpp \
          src/hardware/ethernet_parser/ethernet_parser.cpp

move_file include/datapath/ipv4.hpp \
          include/hardware/ipv4_parser/ipv4.hpp

move_file include/datapath/ipv4_parser.hpp \
          include/hardware/ipv4_parser/ipv4_parser.hpp

move_file src/datapath/ipv4_parser.cpp \
          src/hardware/ipv4_parser/ipv4_parser.cpp

move_file include/datapath/ipv6.hpp \
          include/hardware/ipv6_parser/ipv6.hpp

move_file include/datapath/ipv6_parser.hpp \
          include/hardware/ipv6_parser/ipv6_parser.hpp

move_file src/datapath/ipv6_parser.cpp \
          src/hardware/ipv6_parser/ipv6_parser.cpp

move_file include/datapath/parsed_packet.hpp \
          include/hardware/packet_parser/parsed_packet.hpp

move_file include/datapath/packet_parser.hpp \
          include/hardware/packet_parser/packet_parser.hpp

move_file src/datapath/packet_parser.cpp \
          src/hardware/packet_parser/packet_parser.cpp

# --------------------------------------------------
# 3. Update old include paths in source files
# --------------------------------------------------

find include src -type f \
    \( -name '*.hpp' -o -name '*.cpp' \) \
    -exec sed -i \
    -e 's|datapath/packet_io.hpp|hardware/packet_io/packet_io.hpp|g' \
    -e 's|datapath/packet.hpp|hardware/packet_io/packet.hpp|g' \
    -e 's|datapath/ethernet_parser.hpp|hardware/ethernet_parser/ethernet_parser.hpp|g' \
    -e 's|datapath/ethernet.hpp|hardware/ethernet_parser/ethernet.hpp|g' \
    -e 's|datapath/ipv4_parser.hpp|hardware/ipv4_parser/ipv4_parser.hpp|g' \
    -e 's|datapath/ipv4.hpp|hardware/ipv4_parser/ipv4.hpp|g' \
    -e 's|datapath/ipv6_parser.hpp|hardware/ipv6_parser/ipv6_parser.hpp|g' \
    -e 's|datapath/ipv6.hpp|hardware/ipv6_parser/ipv6.hpp|g' \
    -e 's|datapath/parsed_packet.hpp|hardware/packet_parser/parsed_packet.hpp|g' \
    -e 's|datapath/packet_parser.hpp|hardware/packet_parser/packet_parser.hpp|g' \
    {} +

# --------------------------------------------------
# 4. Remove obsolete directories
# --------------------------------------------------

if [[ -d include/datapath ]]; then
    rmdir include/datapath 2>/dev/null || true
fi

if [[ -d src/datapath ]]; then
    rmdir src/datapath 2>/dev/null || true
fi

# --------------------------------------------------
# 5. Create L2 forwarding interface
# --------------------------------------------------

cat > include/hardware/l2_forwarding/l2_forwarding.hpp <<'EOF'
#pragma once

#include "hardware/packet_parser/parsed_packet.hpp"
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

    L2ForwardingDecision process(
        const ParsedPacket& packet,
        std::uint32_t ingress_port);

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
        std::uint32_t ingress_port) const;

    PortManager& ports_;
    std::map<FdbKey, FdbEntry> fdb_;
};
EOF

# --------------------------------------------------
# 6. Create L2 forwarding implementation
# --------------------------------------------------

cat > src/hardware/l2_forwarding/l2_forwarding.cpp <<'EOF'
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

    const bool all_zero = std::all_of(
        source_mac.begin(),
        source_mac.end(),
        [](std::uint8_t byte) {
            return byte == 0;
        });

    if (all_zero) {
        return false;
    }

    const FdbKey key{vlan_id, source_mac};

    // Updating an existing entry supports MAC movement.
    fdb_[key] = FdbEntry{ingress_port};

    return true;
}

bool L2ForwardingEngine::lookup(
    std::uint16_t vlan_id,
    const MacAddress& destination_mac,
    std::uint32_t& port_id) const
{
    const FdbKey key{vlan_id, destination_mac};
    const auto it = fdb_.find(key);

    if (it == fdb_.end()) {
        return false;
    }

    port_id = it->second.port_id;
    return true;
}

std::vector<std::uint32_t>
L2ForwardingEngine::flood_ports(
    std::uint32_t ingress_port) const
{
    std::vector<std::uint32_t> result;

    for (std::uint32_t id = 0;
         id < ports_.port_count();
         ++id) {

        if (id != ingress_port && is_port_eligible(id)) {
            result.push_back(id);
        }
    }

    return result;
}

L2ForwardingDecision L2ForwardingEngine::process(
    const ParsedPacket& packet,
    std::uint32_t ingress_port)
{
    L2ForwardingDecision decision;

    if (!is_port_eligible(ingress_port)) {
        return decision;
    }

    const auto& ethernet = packet.ethernet;

    const std::uint16_t vlan_id =
        ethernet.vlan_tagged ? ethernet.vlan_id : 0;

    decision.source_learned =
        learn(vlan_id, ethernet.src_mac, ingress_port);

    const MacAddress& destination = ethernet.dst_mac;

    // Multicast and broadcast traffic is flooded.
    if (!is_unicast(destination)) {
        decision.action = L2ForwardingAction::FLOOD;
        decision.egress_ports = flood_ports(ingress_port);
        return decision;
    }

    std::uint32_t destination_port = 0;

    if (!lookup(vlan_id, destination, destination_port) ||
        !is_port_eligible(destination_port)) {

        decision.action = L2ForwardingAction::FLOOD;
        decision.egress_ports = flood_ports(ingress_port);
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
EOF

# --------------------------------------------------
# 7. Create L2 forwarding test declaration
# --------------------------------------------------

cat > include/tests/ipu_l2_forwarding_test.hpp <<'EOF'
#pragma once

#include "core/test.hpp"

class IpuL2ForwardingTest final : public Test {
public:
    std::string name() const override;
    TestResult run() override;
};
EOF

# --------------------------------------------------
# 8. Create L2 forwarding test implementation
# --------------------------------------------------

cat > src/tests/ipu_l2_forwarding_test.cpp <<'EOF'
#include "tests/ipu_l2_forwarding_test.hpp"

#include "hardware/l2_forwarding/l2_forwarding.hpp"

#include <algorithm>
#include <cstdint>

namespace {

MacAddress make_mac(
    std::uint8_t a,
    std::uint8_t b,
    std::uint8_t c,
    std::uint8_t d,
    std::uint8_t e,
    std::uint8_t f)
{
    return {a, b, c, d, e, f};
}

ParsedPacket make_packet(
    const MacAddress& source,
    const MacAddress& destination,
    std::uint16_t vlan = 100)
{
    ParsedPacket packet;
    packet.ethernet.src_mac = source;
    packet.ethernet.dst_mac = destination;
    packet.ethernet.vlan_tagged = true;
    packet.ethernet.vlan_id = vlan;
    return packet;
}

bool contains(
    const std::vector<std::uint32_t>& ports,
    std::uint32_t port)
{
    return std::find(ports.begin(), ports.end(), port)
           != ports.end();
}

} // namespace

std::string IpuL2ForwardingTest::name() const
{
    return "IPU_L2_FORWARDING";
}

TestResult IpuL2ForwardingTest::run()
{
    PortManager ports;

    if (!ports.initialize(4)) {
        return {TestStatus::ERROR, name(),
                "Port initialization failed"};
    }

    for (std::uint32_t id = 0; id < 4; ++id) {
        if (!ports.set_admin_state(id, PortAdminState::UP) ||
            !ports.set_link_present(id, true)) {
            return {TestStatus::ERROR, name(),
                    "Failed to enable ports"};
        }
    }

    L2ForwardingEngine engine(ports);

    const MacAddress mac_a =
        make_mac(0x02, 0, 0, 0, 0, 1);

    const MacAddress mac_b =
        make_mac(0x02, 0, 0, 0, 0, 2);

    const MacAddress mac_c =
        make_mac(0x02, 0, 0, 0, 0, 3);

    // Source learning.
    if (!engine.learn(100, mac_b, 1) ||
        engine.fdb_size() != 1) {
        return {TestStatus::FAIL, name(),
                "MAC learning failed"};
    }

    // Known unicast.
    const auto known =
        engine.process(make_packet(mac_a, mac_b), 0);

    if (known.action != L2ForwardingAction::KNOWN_UNICAST ||
        known.egress_ports.size() != 1 ||
        known.egress_ports[0] != 1) {
        return {TestStatus::FAIL, name(),
                "Known unicast forwarding failed"};
    }

    // Unknown unicast.
    const auto unknown =
        engine.process(make_packet(mac_a, mac_c), 0);

    if (unknown.action != L2ForwardingAction::FLOOD ||
        unknown.egress_ports.size() != 3 ||
        contains(unknown.egress_ports, 0)) {
        return {TestStatus::FAIL, name(),
                "Unknown unicast flooding failed"};
    }

    // VLAN isolation.
    if (!engine.learn(200, mac_b, 2)) {
        return {TestStatus::FAIL, name(),
                "VLAN-specific learning failed"};
    }

    std::uint32_t learned_port = 99;

    if (!engine.lookup(200, mac_b, learned_port) ||
        learned_port != 2) {
        return {TestStatus::FAIL, name(),
                "VLAN-specific lookup failed"};
    }

    // MAC movement.
    if (!engine.learn(100, mac_b, 3)) {
        return {TestStatus::FAIL, name(),
                "MAC movement learning failed"};
    }

    if (!engine.lookup(100, mac_b, learned_port) ||
        learned_port != 3) {
        return {TestStatus::FAIL, name(),
                "MAC movement update failed"};
    }

    // Same-port filtering.
    engine.learn(100, mac_a, 0);

    const auto same_port =
        engine.process(make_packet(mac_c, mac_a), 0);

    if (same_port.action != L2ForwardingAction::DROP ||
        same_port.drop_reason !=
            L2DropReason::DESTINATION_ON_INGRESS_PORT) {
        return {TestStatus::FAIL, name(),
                "Same-port filtering failed"};
    }

    // Broadcast flooding.
    const MacAddress broadcast =
        make_mac(0xFF, 0xFF, 0xFF,
                 0xFF, 0xFF, 0xFF);

    const auto broadcast_result =
        engine.process(make_packet(mac_a, broadcast), 0);

    if (broadcast_result.action != L2ForwardingAction::FLOOD ||
        broadcast_result.egress_ports.size() != 3) {
        return {TestStatus::FAIL, name(),
                "Broadcast flooding failed"};
    }

    // FDB clear.
    engine.clear();

    if (engine.fdb_size() != 0) {
        return {TestStatus::FAIL, name(),
                "FDB clear failed"};
    }

    return {TestStatus::PASS, name(),
            "MAC learning, VLAN isolation, forwarding, flooding, "
            "MAC movement and FDB clear validated"};
}
EOF

# --------------------------------------------------
# 9. Register the test in main.cpp
# --------------------------------------------------

if ! grep -q 'ipu_l2_forwarding_test.hpp' src/main.cpp; then
    sed -i '/#include "tests\/ipu_ipv6_parser_test.hpp"/a #include "tests/ipu_l2_forwarding_test.hpp"' src/main.cpp
fi

if ! grep -q 'IpuL2ForwardingTest' src/main.cpp; then
    sed -i '/IpuIpv6ParserTest()/a\        runner.add(std::make_unique<IpuL2ForwardingTest>());' src/main.cpp
fi

# --------------------------------------------------
# 10. Update CMakeLists.txt source paths
# --------------------------------------------------

sed -i \
    -e 's|src/datapath/packet_io.cpp|src/hardware/packet_io/packet_io.cpp|g' \
    -e 's|src/datapath/ethernet_parser.cpp|src/hardware/ethernet_parser/ethernet_parser.cpp|g' \
    -e 's|src/datapath/ipv4_parser.cpp|src/hardware/ipv4_parser/ipv4_parser.cpp|g' \
    -e 's|src/datapath/ipv6_parser.cpp|src/hardware/ipv6_parser/ipv6_parser.cpp|g' \
    -e 's|src/datapath/packet_parser.cpp|src/hardware/packet_parser/packet_parser.cpp|g' \
    CMakeLists.txt

# Add new source and test if not already present.
if ! grep -q 'src/hardware/l2_forwarding/l2_forwarding.cpp' CMakeLists.txt; then
    sed -i '/add_executable(ipu-simulator/a\    src/hardware/l2_forwarding/l2_forwarding.cpp' CMakeLists.txt
fi

if ! grep -q 'src/tests/ipu_l2_forwarding_test.cpp' CMakeLists.txt; then
    sed -i '/add_executable(ipu-simulator/a\    src/tests/ipu_l2_forwarding_test.cpp' CMakeLists.txt
fi

# --------------------------------------------------
# 11. Verify files and build
# --------------------------------------------------

echo
echo "Checking component files..."

test -f include/hardware/l2_forwarding/l2_forwarding.hpp
test -f src/hardware/l2_forwarding/l2_forwarding.cpp
test -f src/tests/ipu_l2_forwarding_test.cpp

echo "Component files created."

echo
echo "Configuring CMake..."
cmake -S . -B build

echo
echo "Building..."
cmake --build build -j"$(nproc)"

echo
echo "Running L2 forwarding test..."
./build/ipu-simulator --test IPU_L2_FORWARDING

echo
echo "Running complete test suite..."
./build/ipu-simulator --all

echo
echo "======================================"
echo " IPU-008 script completed"
echo "======================================"
