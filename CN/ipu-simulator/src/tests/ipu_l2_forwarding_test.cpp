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
                    "Failed to enable virtual ports"};
        }
    }

    L2ForwardingEngine engine(ports);

    const MacAddress mac_a =
        make_mac(0x02, 0, 0, 0, 0, 1);

    const MacAddress mac_b =
        make_mac(0x02, 0, 0, 0, 0, 2);

    const MacAddress mac_c =
        make_mac(0x02, 0, 0, 0, 0, 3);

    // 1. Learn MAC B on port 1.
    if (!engine.learn(100, mac_b, 1) ||
        engine.fdb_size() != 1) {
        return {TestStatus::FAIL, name(),
                "Source MAC learning failed"};
    }

    // 2. Known unicast: port 0 -> port 1.
    const auto known =
        engine.process(make_packet(mac_a, mac_b), 0);

    if (known.action != L2ForwardingAction::KNOWN_UNICAST ||
        known.egress_ports.size() != 1 ||
        known.egress_ports[0] != 1) {
        return {TestStatus::FAIL, name(),
                "Known-unicast forwarding failed"};
    }

    // 3. Unknown unicast: flood excluding ingress.
    const auto unknown =
        engine.process(make_packet(mac_a, mac_c), 0);

    if (unknown.action != L2ForwardingAction::FLOOD ||
        unknown.egress_ports.size() != 3 ||
        contains(unknown.egress_ports, 0)) {
        return {TestStatus::FAIL, name(),
                "Unknown-unicast flooding failed"};
    }

    // 4. VLAN isolation.
    if (!engine.learn(200, mac_b, 2)) {
        return {TestStatus::FAIL, name(),
                "VLAN-specific MAC learning failed"};
    }

    std::uint32_t learned_port = 99;

    if (!engine.lookup(200, mac_b, learned_port) ||
        learned_port != 2) {
        return {TestStatus::FAIL, name(),
                "VLAN-aware FDB lookup failed"};
    }

    // 5. MAC movement from port 1 to port 3.
    if (!engine.learn(100, mac_b, 3) ||
        !engine.lookup(100, mac_b, learned_port) ||
        learned_port != 3) {
        return {TestStatus::FAIL, name(),
                "MAC movement failed"};
    }

    // 6. Destination is on ingress port.
    engine.learn(100, mac_a, 0);

    const auto same_port =
        engine.process(make_packet(mac_c, mac_a), 0);

    if (same_port.action != L2ForwardingAction::DROP ||
        same_port.drop_reason !=
            L2DropReason::DESTINATION_ON_INGRESS_PORT) {
        return {TestStatus::FAIL, name(),
                "Same-port filtering failed"};
    }

    // 7. Broadcast flooding.
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

    // 8. Clear the FDB.
    engine.clear();

    if (engine.fdb_size() != 0) {
        return {TestStatus::FAIL, name(),
                "FDB clear failed"};
    }

    return {TestStatus::PASS, name(),
            "MAC learning, VLAN isolation, known/unknown unicast, "
            "MAC movement, broadcast and FDB clear validated"};
}
