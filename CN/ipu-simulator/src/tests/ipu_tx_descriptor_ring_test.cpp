#include "tests/ipu_tx_descriptor_ring_test.hpp"

#include "hardware/packet_buffer/buffer_pool.hpp"
#include "hardware/tx_descriptor_ring/tx_descriptor_ring.hpp"

#include <array>
#include <cstdint>
#include <string>

std::string IpuTxDescriptorRingTest::name() const
{
    return "IPU_TX_DESCRIPTOR_RING";
}

TestResult IpuTxDescriptorRingTest::run()
{
    constexpr std::size_t RING_SIZE = 4;
    constexpr std::size_t POOL_SIZE = 5;
    constexpr std::size_t BUFFER_CAPACITY = 512;

    const auto fail = [this](const std::string& message) {
        return TestResult{TestStatus::FAIL, name(), message};
    };

    PacketBufferPool pool(POOL_SIZE, BUFFER_CAPACITY, 128);
    TxDescriptorRing ring(RING_SIZE);

    if (ring.ring_size() != RING_SIZE ||
        pool.available_count() != POOL_SIZE) {
        return fail("Incorrect initial state");
    }

    std::array<std::array<std::uint8_t, 64>, RING_SIZE> frames{};

    for (std::size_t packet = 0; packet < RING_SIZE; ++packet) {
        for (std::size_t i = 0; i < frames[packet].size(); ++i) {
            frames[packet][i] =
                static_cast<std::uint8_t>(packet * 16 + i);
        }

        PacketBufferLease lease = pool.acquire_lease();

        if (!lease || !lease->append(
                frames[packet].data(),
                frames[packet].size())) {
            return fail("Unable to prepare TX packet buffer");
        }

        PacketMetadata metadata;
        metadata.packet_id = 500 + packet;
        metadata.egress_port = 1;

        if (!ring.submit(std::move(lease), metadata)) {
            return fail("Unable to submit TX descriptor");
        }

        if (lease) {
            return fail("TX submission did not transfer buffer ownership");
        }
    }

    if (ring.posted_count() != RING_SIZE ||
        ring.producer_index() != 0 ||
        pool.in_use_count() != RING_SIZE) {
        return fail("TX ring posting or producer wraparound failed");
    }

    // Ring is full. The extra lease must remain with the caller.
    PacketBufferLease extra = pool.acquire_lease();

    if (!extra || !extra->append(frames[0].data(), frames[0].size())) {
        return fail("Unable to prepare extra packet");
    }

    PacketMetadata extra_metadata;
    extra_metadata.packet_id = 999;

    if (ring.submit(std::move(extra), extra_metadata)) {
        return fail("Submission succeeded on a full ring");
    }

    if (!extra) {
        return fail("Failed submission consumed caller's buffer lease");
    }

    extra.reset();

    // Simulate device reading all four packets.
    for (std::size_t i = 0; i < RING_SIZE; ++i) {
        if (!ring.simulate_dma_transmit()) {
            return fail("Simulated DMA transmit failed");
        }
    }

    if (ring.completed_count() != RING_SIZE ||
        ring.posted_count() != 0 ||
        ring.device_index() != 0 ||
        ring.transmitted_count() != RING_SIZE) {
        return fail("Incorrect TX completion state");
    }

    // No posted descriptor remains.
    if (ring.simulate_dma_transmit()) {
        return fail("Device transmitted without a posted descriptor");
    }

    // Validate transmitted frame contents and ordering.
    for (std::size_t packet = 0; packet < RING_SIZE; ++packet) {
        auto transmitted = ring.take_transmitted_frame();

        if (!transmitted.has_value() ||
            transmitted->size() != frames[packet].size()) {
            return fail("Transmitted frame length or availability incorrect");
        }

        for (std::size_t i = 0; i < frames[packet].size(); ++i) {
            if ((*transmitted)[i] != frames[packet][i]) {
                return fail("Transmitted payload differs from submitted data");
            }
        }
    }

    if (ring.take_transmitted_frame().has_value()) {
        return fail("Unexpected extra transmitted frame");
    }

    // Poll completions and recover software ownership of the buffers.
    for (std::size_t packet = 0; packet < RING_SIZE; ++packet) {
        auto completion = ring.poll();

        if (!completion.has_value()) {
            return fail("Unable to poll TX completion");
        }

        if (completion->status != TxCompletionStatus::SUCCESS ||
            completion->metadata.packet_id != 500 + packet ||
            completion->metadata.egress_port != 1 ||
            !completion->buffer ||
            completion->buffer->size() != frames[packet].size()) {
            return fail("TX completion metadata or buffer is incorrect");
        }

        completion.reset();
    }

    if (ring.completed_count() != 0 ||
        ring.consumer_index() != 0 ||
        pool.in_use_count() != 0 ||
        pool.available_count() != POOL_SIZE) {
        return fail("TX completion did not recycle all buffers");
    }

    // Reuse the ring after wraparound.
    PacketBufferLease reuse = pool.acquire_lease();

    if (!reuse || !reuse->append(frames[0].data(), frames[0].size())) {
        return fail("Unable to acquire buffer for ring reuse");
    }

    PacketMetadata reuse_metadata;
    reuse_metadata.packet_id = 700;

    if (!ring.submit(std::move(reuse), reuse_metadata) ||
        !ring.simulate_dma_transmit()) {
        return fail("TX ring reuse failed");
    }

    auto reuse_completion = ring.poll();

    if (!reuse_completion.has_value() ||
        reuse_completion->metadata.packet_id != 700) {
        return fail("TX ring reuse completion failed");
    }

    reuse_completion.reset();

    // Reset with a descriptor still owned by the device.
    PacketBufferLease pending = pool.acquire_lease();

    if (!pending || !pending->append(frames[1].data(), frames[1].size())) {
        return fail("Unable to prepare pending packet");
    }

    if (!ring.submit(std::move(pending), reuse_metadata)) {
        return fail("Unable to submit pending packet");
    }

    ring.reset();

    if (ring.posted_count() != 0 ||
        ring.completed_count() != 0 ||
        ring.producer_index() != 0 ||
        ring.device_index() != 0 ||
        ring.consumer_index() != 0 ||
        pool.in_use_count() != 0 ||
        pool.available_count() != POOL_SIZE) {
        return fail("TX reset did not restore initial state");
    }

    return {
        TestStatus::PASS,
        name(),
        "TX submission, ownership transfer, simulated DMA reads, frame capture, completion, wraparound and reset validated"
    };
}
