#include "tests/ipu_packet_io_test.hpp"
#include "ipu/ipu_simulator.hpp"

#include <cstdint>
#include <string>
#include <vector>

std::string IpuPacketIoTest::name() const
{
    return "IPU_PACKET_IO";
}

TestResult IpuPacketIoTest::run()
{
    IpuSimulator ipu;

    const auto fail = [&](const std::string& reason) {
        ipu.shutdown();

        return TestResult{
            TestStatus::FAIL,
            name(),
            reason
        };
    };

    if (!ipu.initialize()) {
        return fail("IPU initialization failed");
    }

    PortManager& ports = ipu.ports();
    PacketIoEngine& io = ipu.packet_io();

    // Configure port 0.
    if (!ports.set_admin_state(0, PortAdminState::UP)) {
        return fail("Unable to enable port 0");
    }

    if (!ports.set_link_present(0, true)) {
        return fail("Unable to activate port 0 link");
    }

    if (!io.set_loopback(0, true)) {
        return fail("Unable to enable port loopback");
    }

    // Construct a 64-byte Ethernet frame.
    Packet tx_packet;

    tx_packet.data = {
        0x02, 0x00, 0x00, 0x00, 0x00, 0x02, // Destination MAC
        0x02, 0x00, 0x00, 0x00, 0x00, 0x01, // Source MAC
        0x08, 0x00                          // EtherType: IPv4
    };

    tx_packet.data.resize(64, 0);

    const std::vector<std::uint8_t> original_data = tx_packet.data;

    // Inject packet.
    if (!io.inject_packet(0, tx_packet)) {
        return fail("Packet injection failed");
    }

    if (io.pending_ingress() != 1) {
        return fail("Unexpected ingress queue depth");
    }

    // Process one packet.
    const std::size_t processed = io.process_packets(1);

    if (processed != 1) {
        return fail("Packet processing budget did not process one packet");
    }

    // Device transmission and software reclamation are separate stages.
    if (io.tx_posted_descriptors() != 1 || io.pending_egress() != 0) {
        return fail("TX submission state is incorrect");
    }

    if (io.service_tx(1) != 1 || io.tx_completed_descriptors() != 1) {
        return fail("Device did not complete the TX descriptor");
    }

    if (io.pending_egress() != 0) {
        return fail("Frame became visible before completion reclamation");
    }

    if (io.reclaim_tx(1) != 1) {
        return fail("TX completion reclamation failed");
    }

    if (io.pending_ingress() != 0) {
        return fail("Ingress queue was not drained");
    }

    if (io.pending_egress() != 1) {
        return fail("Packet did not reach egress capture queue");
    }

    // Capture packet.
    auto rx_packet = io.capture_packet(0);

    if (!rx_packet.has_value()) {
        return fail("Unable to capture egress packet");
    }

    if (rx_packet->data != original_data) {
        return fail("Packet contents changed during loopback");
    }

    if (rx_packet->ingress_port != 0 ||
        rx_packet->egress_port != 0) {
        return fail("Incorrect packet ingress or egress metadata");
    }

    if (rx_packet->packet_id == 0) {
        return fail("Invalid packet identifier");
    }

    if (io.pending_egress() != 0) {
        return fail("Egress queue was not drained after capture");
    }

    // Validate port statistics.
    const EthernetPort* port = ports.get_port(0);

    if (port == nullptr) {
        return fail("Port 0 disappeared");
    }

    if (port->stats.rx_packets != 1 ||
        port->stats.tx_packets != 1) {
        return fail("Incorrect RX/TX packet counters");
    }

    if (port->stats.rx_bytes != original_data.size() ||
        port->stats.tx_bytes != original_data.size()) {
        return fail("Incorrect RX/TX byte counters");
    }

    // A port with link DOWN must reject packet injection.
    if (!ports.set_link_present(0, false)) {
        return fail("Unable to deactivate port 0 link");
    }

    if (io.inject_packet(0, tx_packet)) {
        return fail("Packet accepted while link was DOWN");
    }

    if (io.pending_ingress() != 0) {
        return fail("Rejected packet unexpectedly entered ingress queue");
    }

    if (port->stats.rx_dropped != 1) {
        return fail("Incorrect RX dropped counter");
    }

    ipu.shutdown();

    return {
        TestStatus::PASS,
        name(),
        "Packet injection, processing, loopback, capture and counters validated"
    };
}
