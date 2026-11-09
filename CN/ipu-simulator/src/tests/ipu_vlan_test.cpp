#include "tests/ipu_vlan_test.hpp"

#include "ipu/ipu_simulator.hpp"

#include <cstdint>
#include <vector>

namespace {

Packet make_packet(
    const MacAddress& destination,
    const MacAddress& source,
    bool tagged = false,
    std::uint16_t vlan_id = 0)
{
    Packet packet;
    packet.data.resize(60, 0);

    for (std::size_t i = 0; i < 6; ++i) {
        packet.data[i] = destination[i];
        packet.data[6 + i] = source[i];
    }

    packet.data[12] = 0x08;
    packet.data[13] = 0x06;

    if (tagged) {
        const std::uint16_t tci = vlan_id & 0x0FFFU;

        packet.data.insert(
            packet.data.begin() + 12,
            {
                0x81,
                0x00,
                static_cast<std::uint8_t>(tci >> 8U),
                static_cast<std::uint8_t>(tci & 0xFFU)
            });
    }

    return packet;
}

bool has_vlan_tag(const Packet& packet, std::uint16_t vlan_id)
{
    if (packet.data.size() < 18 ||
        packet.data[12] != 0x81 ||
        packet.data[13] != 0x00) {
        return false;
    }

    const std::uint16_t actual_vlan =
        static_cast<std::uint16_t>(
            ((static_cast<std::uint16_t>(packet.data[14]) << 8U) |
             packet.data[15]) & 0x0FFFU);

    return actual_vlan == vlan_id;
}

} // namespace

std::string IpuVlanTest::name() const
{
    return "IPU_VLAN";
}

TestResult IpuVlanTest::run()
{
    IpuSimulator device;

    if (!device.initialize()) {
        return {TestStatus::ERROR, name(),
                "IPU initialization failed"};
    }

    for (std::uint32_t id = 0; id < 4; ++id) {
        if (!device.ports().set_admin_state(id, PortAdminState::UP) ||
            !device.ports().set_link_present(id, true)) {
            return {TestStatus::ERROR, name(),
                    "Port activation failed"};
        }
    }

    VlanManager& vlans = device.datapath().vlans();

    if (!vlans.create_vlan(10) ||
        !vlans.create_vlan(20) ||
        !vlans.create_vlan(30)) {
        return {TestStatus::FAIL, name(),
                "VLAN creation failed"};
    }

    if (!vlans.configure_access_port(0, 10) ||
        !vlans.configure_access_port(1, 10) ||
        !vlans.configure_access_port(2, 20) ||
        !vlans.configure_trunk_port(3, {10, 20})) {
        return {TestStatus::FAIL, name(),
                "Port VLAN configuration failed"};
    }

    const MacAddress broadcast{
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    const MacAddress mac0{
        0x02, 0x00, 0x00, 0x00, 0x00, 0x10
    };

    const MacAddress mac1{
        0x02, 0x00, 0x00, 0x00, 0x00, 0x11
    };

    const MacAddress mac2{
        0x02, 0x00, 0x00, 0x00, 0x00, 0x12
    };

    // Test 1: Broadcast from access VLAN 10.
    Packet vlan10_broadcast = make_packet(broadcast, mac0);

    if (!device.packet_io().inject_packet(0, vlan10_broadcast) ||
        device.process_packets(8) != 1) {
        return {TestStatus::FAIL, name(),
                "VLAN 10 broadcast processing failed"};
    }

    if (device.packet_io().service_tx(8) != 2 ||
        device.packet_io().reclaim_tx(8) != 2) {
        return {TestStatus::FAIL, name(),
                "VLAN 10 broadcast TX service or reclamation failed"};
    }

    const auto access_output =
        device.packet_io().capture_packet(1);

    const auto trunk_output =
        device.packet_io().capture_packet(3);

    if (!access_output || !trunk_output) {
        return {TestStatus::FAIL, name(),
                "VLAN 10 broadcast did not reach expected ports"};
    }

    if (access_output->data.size() != 60 ||
        access_output->data[12] != 0x08 ||
        access_output->data[13] != 0x06) {
        return {TestStatus::FAIL, name(),
                "Access egress should be untagged"};
    }

    if (!has_vlan_tag(*trunk_output, 10) ||
        trunk_output->data.size() != 64) {
        return {TestStatus::FAIL, name(),
                "Trunk egress should carry VLAN 10 tag"};
    }

    if (device.packet_io().capture_packet(2).has_value()) {
        return {TestStatus::FAIL, name(),
                "VLAN 10 traffic leaked into VLAN 20"};
    }

    // Test 2: Broadcast from access VLAN 20.
    Packet vlan20_broadcast = make_packet(broadcast, mac2);

    if (!device.packet_io().inject_packet(2, vlan20_broadcast) ||
        device.process_packets(8) != 1) {
        return {TestStatus::FAIL, name(),
                "VLAN 20 broadcast processing failed"};
    }

    if (device.packet_io().service_tx(8) != 1 ||
        device.packet_io().reclaim_tx(8) != 1) {
        return {TestStatus::FAIL, name(),
                "VLAN 20 broadcast TX service or reclamation failed"};
    }

    const auto vlan20_trunk =
        device.packet_io().capture_packet(3);

    if (!vlan20_trunk || !has_vlan_tag(*vlan20_trunk, 20)) {
        return {TestStatus::FAIL, name(),
                "VLAN 20 trunk forwarding failed"};
    }

    if (device.packet_io().capture_packet(0).has_value() ||
        device.packet_io().capture_packet(1).has_value()) {
        return {TestStatus::FAIL, name(),
                "VLAN 20 traffic leaked into VLAN 10"};
    }

    // Test 3: Tagged VLAN 10 ingress on trunk.
    Packet trunk_vlan10 =
        make_packet(broadcast, mac1, true, 10);

    if (!device.packet_io().inject_packet(3, trunk_vlan10) ||
        device.process_packets(8) != 1) {
        return {TestStatus::FAIL, name(),
                "Tagged trunk ingress failed"};
    }

    if (device.packet_io().service_tx(8) != 2 ||
        device.packet_io().reclaim_tx(8) != 2) {
        return {TestStatus::FAIL, name(),
                "Trunk-to-access TX service or reclamation failed"};
    }

    const auto trunk_to_access0 =
        device.packet_io().capture_packet(0);

    const auto trunk_to_access1 =
        device.packet_io().capture_packet(1);

    if (!trunk_to_access0 || !trunk_to_access1) {
        return {TestStatus::FAIL, name(),
                "Trunk VLAN 10 did not reach access ports"};
    }

    if (trunk_to_access0->data.size() != 60 ||
        trunk_to_access1->data.size() != 60 ||
        trunk_to_access0->data[12] != 0x08 ||
        trunk_to_access1->data[12] != 0x08) {
        return {TestStatus::FAIL, name(),
                "Trunk-to-access egress should remove VLAN tags"};
    }

    // Test 4: Known unicast within VLAN 10.
    Packet known_unicast = make_packet(mac0, mac1);

    if (!device.packet_io().inject_packet(1, known_unicast) ||
        device.process_packets(8) != 1) {
        return {TestStatus::FAIL, name(),
                "Known-unicast processing failed"};
    }

    if (device.packet_io().service_tx(8) != 1 ||
        device.packet_io().reclaim_tx(8) != 1) {
        return {TestStatus::FAIL, name(),
                "Known-unicast VLAN TX service or reclamation failed"};
    }

    const auto known_output =
        device.packet_io().capture_packet(0);

    if (!known_output || known_output->egress_port != 0) {
        return {TestStatus::FAIL, name(),
                "Known-unicast forwarding selected wrong port"};
    }

    // Test 5: Reject VLAN 30 on trunk port 3.
    Packet unauthorized =
        make_packet(broadcast, mac1, true, 30);

    const EthernetPort* trunk_port =
        device.ports().get_port(3);

    if (trunk_port == nullptr) {
        return {TestStatus::ERROR, name(),
                "Trunk port lookup failed"};
    }

    const std::uint64_t drops_before =
        trunk_port->stats.rx_dropped;

    if (!device.packet_io().inject_packet(3, unauthorized)) {
        return {TestStatus::FAIL, name(),
                "Unauthorized frame injection failed"};
    }

    device.process_packets(8);

    if (trunk_port->stats.rx_dropped != drops_before + 1) {
        return {TestStatus::FAIL, name(),
                "Unauthorized VLAN was not dropped"};
    }

    // Test 6: Reject tagged ingress on access port.
    Packet tagged_access =
        make_packet(broadcast, mac0, true, 10);

    const EthernetPort* access_port =
        device.ports().get_port(0);

    if (access_port == nullptr) {
        return {TestStatus::ERROR, name(),
                "Access port lookup failed"};
    }

    const std::uint64_t access_drops_before =
        access_port->stats.rx_dropped;

    if (!device.packet_io().inject_packet(0, tagged_access)) {
        return {TestStatus::FAIL, name(),
                "Tagged access frame injection failed"};
    }

    device.process_packets(8);

    if (access_port->stats.rx_dropped != access_drops_before + 1) {
        return {TestStatus::FAIL, name(),
                "Tagged access frame was not rejected"};
    }

    // Test 7: Reject untagged ingress on trunk.
    Packet untagged_trunk = make_packet(broadcast, mac1);

    const std::uint64_t trunk_drops_before =
        trunk_port->stats.rx_dropped;

    if (!device.packet_io().inject_packet(3, untagged_trunk)) {
        return {TestStatus::FAIL, name(),
                "Untagged trunk frame injection failed"};
    }

    device.process_packets(8);

    if (trunk_port->stats.rx_dropped != trunk_drops_before + 1) {
        return {TestStatus::FAIL, name(),
                "Untagged trunk frame was not rejected"};
    }

    device.shutdown();

    return {
        TestStatus::PASS,
        name(),
        "VLAN database, access/trunk classification, VLAN isolation, "
        "FDB forwarding, broadcast filtering and egress tagging validated"
    };
}
