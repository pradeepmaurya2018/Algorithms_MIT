#include "tests/ipu_packet_buffer_io_test.hpp"

#include "hardware/packet_io/packet_io.hpp"
#include "ipu/port_manager.hpp"

#include <cstdint>
#include <string>

std::string IpuPacketBufferIoTest::name() const
{
    return "IPU_PACKET_BUFFER_IO";
}

TestResult IpuPacketBufferIoTest::run()
{
    PortManager ports;

    if (!ports.initialize(1)) {
        return {TestStatus::ERROR, name(), "Port initialization failed"};
    }

    if (!ports.set_admin_state(0, PortAdminState::UP) ||
        !ports.set_link_present(0, true)) {
        return {TestStatus::ERROR, name(), "Port configuration failed"};
    }

    constexpr std::size_t POOL_SIZE = 2;
    constexpr std::size_t BUFFER_CAPACITY = 2048;

    PacketIoEngine io(ports, POOL_SIZE, BUFFER_CAPACITY);

    const auto fail = [this](const std::string& message) {
        return TestResult{TestStatus::FAIL, name(), message};
    };

    if (io.buffers_available() != POOL_SIZE ||
        io.buffers_in_use() != 0) {
        return fail("Incorrect initial pool state");
    }

    if (!io.set_loopback(0, true)) {
        return fail("Unable to enable loopback");
    }

    Packet packet;
    packet.data = {
        0x02, 0x00, 0x00, 0x00, 0x00, 0x02,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x01,
        0x08, 0x00
    };
    packet.data.resize(64, 0);

    // First injection acquires one buffer.
    if (!io.inject_packet(0, packet)) {
        return fail("First packet injection failed");
    }

    if (io.buffers_available() != 1 ||
        io.buffers_in_use() != 1) {
        return fail("First injection did not acquire one buffer");
    }

    // Second injection exhausts the pool.
    if (!io.inject_packet(0, packet)) {
        return fail("Second packet injection failed");
    }

    if (io.buffers_available() != 0 ||
        io.buffers_in_use() != 2) {
        return fail("Pool exhaustion state is incorrect");
    }

    // Third injection must fail without allocating another buffer.
    if (io.inject_packet(0, packet)) {
        return fail("Injection succeeded despite pool exhaustion");
    }

    if (io.buffers_in_use() != 2 ||
        io.buffers_available() != 0) {
        return fail("Pool state changed after exhausted injection");
    }

    const EthernetPort* port = ports.get_port(0);

    if (port == nullptr || port->stats.rx_dropped != 1) {
        return fail("Pool exhaustion did not increment RX drop counter");
    }

    // Processing one packet releases its ingress buffer.
    if (io.process_packets(1) != 1) {
        return fail("Failed to process first loopback packet");
    }

    // The first TX buffer remains allocated until completion reclamation.
    if (io.buffers_in_use() != 2 ||
        io.tx_posted_descriptors() != 1) {
        return fail("First TX submission did not retain its buffer");
    }

    if (io.service_tx(1) != 1 ||
        io.tx_completed_descriptors() != 1) {
        return fail("First TX device service failed");
    }

    if (io.reclaim_tx(1) != 1) {
        return fail("First TX completion reclamation failed");
    }

    if (io.buffers_available() != 1 ||
        io.buffers_in_use() != 1) {
        return fail("First processed packet did not recycle its buffer");
    }

    // Processing the second packet releases the remaining buffer.
    if (io.process_packets(1) != 1) {
        return fail("Failed to process second loopback packet");
    }

    if (io.tx_posted_descriptors() != 1) {
        return fail("Second TX descriptor was not posted");
    }

    if (io.service_tx(1) != 1 ||
        io.reclaim_tx(1) != 1) {
        return fail("Second TX service or reclamation failed");
    }

    if (io.buffers_available() != 2 ||
        io.buffers_in_use() != 0) {
        return fail("Second processed packet did not recycle its buffer");
    }

    // Egress packets are legacy Packet objects, not pool leases.
    if (io.pending_egress() != 2) {
        return fail("Unexpected egress queue depth");
    }

    // Capture both packets.
    if (!io.capture_packet(0).has_value() ||
        !io.capture_packet(0).has_value()) {
        return fail("Unable to capture loopback packets");
    }

    // Verify reset releases buffers still held by ingress entries.
    if (!io.inject_packet(0, packet) ||
        !io.inject_packet(0, packet)) {
        return fail("Unable to refill pool before reset test");
    }

    if (io.buffers_in_use() != 2) {
        return fail("Expected two buffers in use before reset");
    }

    io.reset();

    if (io.pending_ingress() != 0 ||
        io.pending_egress() != 0 ||
        io.buffers_available() != 2 ||
        io.buffers_in_use() != 0) {
        return fail("Reset did not release all queued buffers");
    }

    // Empty packets must be rejected before acquiring a buffer.
    Packet empty_packet;

    if (io.inject_packet(0, empty_packet)) {
        return fail("Empty packet was accepted");
    }

    if (io.buffers_available() != 2 ||
        io.buffers_in_use() != 0) {
        return fail("Invalid packet consumed a pool buffer");
    }

    return {
        TestStatus::PASS,
        name(),
        "Pool allocation, exhaustion, RX recycling, reset and invalid input validated"
    };
}
