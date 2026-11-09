#include "tests/ipu_packet_io_datapath_test.hpp"

#include "ipu/ipu_simulator.hpp"

#include <cstdint>
#include <vector>

namespace {

Packet make_ethernet_packet(
    const MacAddress& source,
    const MacAddress& destination,
    std::uint32_t ingress_port)
{
    Packet packet;
    packet.data.resize(60, 0);

    for (std::size_t i = 0; i < 6; ++i) {
        packet.data[i] = destination[i];
        packet.data[6 + i] = source[i];
    }

    // ARP EtherType.
    packet.data[12] = 0x08;
    packet.data[13] = 0x06;
    packet.ingress_port = ingress_port;

    return packet;
}

} // namespace

std::string IpuPacketIoDatapathTest::name() const
{
    return "IPU_DEVICE_DATAPATH";
}

TestResult IpuPacketIoDatapathTest::run()
{
    // One device owns all internal hardware components.
    IpuSimulator device;

    if (!device.initialize()) {
        return {TestStatus::ERROR, name(),
                "IPU simulator initialization failed"};
    }

    // Configure virtual ports through the device interface.
    for (std::uint32_t id = 0; id < device.info().port_count; ++id) {
        if (!device.ports().set_admin_state(id, PortAdminState::UP) ||
            !device.ports().set_link_present(id, true)) {
            return {TestStatus::ERROR, name(),
                    "Failed to enable virtual ports"};
        }
    }

    const MacAddress source{
        0x02, 0x00, 0x00, 0x00, 0x00, 0x01
    };

    const MacAddress destination{
        0x02, 0x00, 0x00, 0x00, 0x00, 0x02
    };

    // Learn destination MAC on port 1, VLAN 0.
    if (!device.datapath().forwarding().learn(
            0, destination, 1)) {
        return {TestStatus::FAIL, name(),
                "Failed to learn destination MAC"};
    }

    const Packet input =
        make_ethernet_packet(source, destination, 0);

    // Inject through the device's packet I/O engine.
    if (!device.packet_io().inject_packet(0, input)) {
        return {TestStatus::FAIL, name(),
                "Packet injection failed"};
    }

    if (device.packet_io().pending_ingress() != 1) {
        return {TestStatus::FAIL, name(),
                "Unexpected ingress queue depth"};
    }

    // Process through the device-owned datapath.
    const std::size_t processed = device.process_packets(8);

    if (processed != 1) {
        return {TestStatus::FAIL, name(),
                "Unexpected number of processed packets"};
    }

    // Known-unicast packet is submitted but not yet transmitted.
    if (device.packet_io().tx_posted_descriptors() != 1 ||
        device.packet_io().pending_egress() != 0) {
        return {TestStatus::FAIL, name(),
                "Unexpected TX submission state"};
    }

    if (device.packet_io().service_tx(1) != 1 ||
        device.packet_io().tx_completed_descriptors() != 1) {
        return {TestStatus::FAIL, name(),
                "Known-unicast TX service failed"};
    }

    if (device.packet_io().pending_egress() != 0 ||
        device.packet_io().reclaim_tx(1) != 1) {
        return {TestStatus::FAIL, name(),
                "Known-unicast TX reclamation failed"};
    }

    if (device.packet_io().pending_ingress() != 0 ||
        device.packet_io().pending_egress() != 1) {
        return {TestStatus::FAIL, name(),
                "Unexpected queue state after forwarding"};
    }

    // Capture from egress port 1.
    const auto captured = device.packet_io().capture_packet(1);

    if (!captured.has_value()) {
        return {TestStatus::FAIL, name(),
                "Packet capture on port 1 failed"};
    }

    if (captured->ingress_port != 0 ||
        captured->egress_port != 1 ||
        captured->data != input.data ||
        captured->packet_id == 0) {
        return {TestStatus::FAIL, name(),
                "Captured packet metadata or data incorrect"};
    }

    // Validate port statistics.
    const EthernetPort* ingress = device.ports().get_port(0);
    const EthernetPort* egress = device.ports().get_port(1);

    if (ingress == nullptr || egress == nullptr) {
        return {TestStatus::ERROR, name(),
                "Port lookup failed"};
    }

    if (ingress->stats.rx_packets != 1 ||
        ingress->stats.rx_bytes != input.size() ||
        egress->stats.tx_packets != 1 ||
        egress->stats.tx_bytes != input.size()) {
        return {TestStatus::FAIL, name(),
                "Ingress or egress statistics incorrect"};
    }

    // Test broadcast flooding through the integrated RX/TX rings.
    const MacAddress broadcast{
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    const MacAddress flood_source{
        0x02, 0x00, 0x00, 0x00, 0x00, 0x03
    };

    const Packet flood_input =
        make_ethernet_packet(flood_source, broadcast, 0);

    if (!device.packet_io().inject_packet(0, flood_input)) {
        return {TestStatus::FAIL, name(),
                "Broadcast packet injection failed"};
    }

    if (device.packet_io().pending_ingress() != 1) {
        return {TestStatus::FAIL, name(),
                "Broadcast RX completion missing"};
    }

    if (device.process_packets(8) != 1) {
        return {TestStatus::FAIL, name(),
                "Broadcast datapath processing failed"};
    }

    // Three independent egress copies must occupy three TX descriptors.
    if (device.packet_io().tx_posted_descriptors() != 3 ||
        device.packet_io().pending_egress() != 0) {
        return {TestStatus::FAIL, name(),
                "Broadcast TX submission state is incorrect"};
    }

    if (device.packet_io().service_tx(3) != 3 ||
        device.packet_io().tx_completed_descriptors() != 3) {
        return {TestStatus::FAIL, name(),
                "Broadcast TX device service failed"};
    }

    if (device.packet_io().pending_egress() != 0 ||
        device.packet_io().reclaim_tx(3) != 3) {
        return {TestStatus::FAIL, name(),
                "Broadcast TX completion reclamation failed"};
    }

    if (device.packet_io().pending_ingress() != 0 ||
        device.packet_io().pending_egress() != 3) {
        return {TestStatus::FAIL, name(),
                "Broadcast did not produce three egress packets"};
    }

    for (std::uint32_t port_id = 1; port_id < 4; ++port_id) {
        const auto output =
            device.packet_io().capture_packet(port_id);

        if (!output) {
            return {TestStatus::FAIL, name(),
                    "Broadcast capture failed on egress port " +
                    std::to_string(port_id)};
        }

        if (output->data != flood_input.data ||
            output->ingress_port != 0 ||
            output->egress_port != port_id ||
            output->packet_id == 0) {
            return {TestStatus::FAIL, name(),
                    "Broadcast egress data or metadata incorrect"};
        }
    }

    if (device.packet_io().pending_egress() != 0) {
        return {TestStatus::FAIL, name(),
                "Unexpected packets remain in egress capture queue"};
    }

    if (device.packet_io().buffers_in_use() != 0 ||
        device.packet_io().rx_posted_descriptors() != 0 ||
        device.packet_io().rx_completed_descriptors() != 0 ||
        device.packet_io().tx_posted_descriptors() != 0 ||
        device.packet_io().tx_completed_descriptors() != 0) {
        return {TestStatus::FAIL, name(),
                "RX/TX descriptors or packet buffers were not recycled"};
    }

    // Verify malformed packet handling.
    Packet malformed;
    malformed.data.resize(5, 0);

    if (!device.packet_io().inject_packet(0, malformed)) {
        return {TestStatus::FAIL, name(),
                "Malformed packet should enter ingress queue"};
    }

    device.process_packets(8);

    if (device.packet_io().pending_egress() != 0 ||
        ingress->stats.rx_dropped != 1) {
        return {TestStatus::FAIL, name(),
                "Malformed packet was not dropped correctly"};
    }

    device.shutdown();

    if (device.is_initialized()) {
        return {TestStatus::FAIL, name(),
                "Device shutdown failed"};
    }

    return {
        TestStatus::PASS,
        name(),
        "Device lifecycle, packet injection, integrated datapath, "
        "FDB forwarding, egress capture, statistics and packet drops "
        "validated"
    };
}
