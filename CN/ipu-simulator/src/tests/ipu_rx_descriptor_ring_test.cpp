#include "tests/ipu_rx_descriptor_ring_test.hpp"

#include "hardware/packet_buffer/buffer_pool.hpp"
#include "hardware/rx_descriptor_ring/rx_descriptor_ring.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

std::string IpuRxDescriptorRingTest::name() const
{
    return "IPU_RX_DESCRIPTOR_RING";
}

TestResult IpuRxDescriptorRingTest::run()
{
    constexpr std::size_t RING_SIZE = 4;
    constexpr std::size_t POOL_SIZE = 4;
    constexpr std::size_t BUFFER_CAPACITY = 512;

    const auto fail = [this](const std::string& message) {
        return TestResult{TestStatus::FAIL, name(), message};
    };

    PacketBufferPool pool(POOL_SIZE, BUFFER_CAPACITY, 128);
    RxDescriptorRing ring(pool, RING_SIZE);

    if (ring.ring_size() != RING_SIZE ||
        pool.available_count() != POOL_SIZE) {
        return fail("Incorrect initial ring or pool state");
    }

    if (ring.post_all() != RING_SIZE) {
        return fail("Unable to post all RX descriptors");
    }

    if (ring.posted_count() != RING_SIZE ||
        pool.in_use_count() != POOL_SIZE) {
        return fail("Incorrect posted descriptor count");
    }

    const auto posted = ring.descriptor_info(0);

    if (!posted.has_value() ||
        posted->state != RxDescriptorState::POSTED ||
        posted->owner != RxDescriptorOwner::DEVICE ||
        posted->completion_status != RxCompletionStatus::NONE ||
        posted->dma_address == 0 ||
        posted->buffer_capacity == 0 ||
        posted->received_length != 0) {
        return fail("Posted descriptor fields are incorrect");
    }

    if (ring.post()) {
        return fail("Posting succeeded with exhausted pool");
    }

    std::array<std::uint8_t, 64> frame{};

    for (std::size_t i = 0; i < frame.size(); ++i) {
        frame[i] = static_cast<std::uint8_t>(i);
    }

    PacketMetadata metadata;
    metadata.packet_id = 100;
    metadata.ingress_port = 2;
    metadata.ether_type = 0x0800;
    metadata.is_ipv4 = true;

    for (std::size_t i = 0; i < RING_SIZE; ++i) {
        metadata.packet_id = 100 + i;

        if (!ring.simulate_dma_receive(frame, metadata)) {
            return fail("DMA receive failed for a posted descriptor");
        }
    }

    if (ring.completed_count() != RING_SIZE ||
        ring.posted_count() != 0) {
        return fail("Incorrect descriptor completion state");
    }

    const auto completed = ring.descriptor_info(0);

    if (!completed.has_value() ||
        completed->state != RxDescriptorState::COMPLETED ||
        completed->owner != RxDescriptorOwner::SOFTWARE ||
        completed->completion_status != RxCompletionStatus::SUCCESS ||
        completed->received_length != frame.size()) {
        return fail("Hardware-style completion fields are incorrect");
    }

    if (ring.device_index() != 0) {
        return fail("Device index did not wrap around");
    }

    if (ring.simulate_dma_receive(frame, metadata)) {
        return fail("DMA overwrote a completed descriptor");
    }

    auto received = ring.poll();

    if (!received.has_value()) {
        return fail("Unable to poll completed descriptor");
    }

    if (received->metadata.packet_id != 100 ||
        received->metadata.ingress_port != 2 ||
        !received->metadata.is_ipv4) {
        return fail("Packet metadata was not preserved");
    }

    if (received->buffer->size() != frame.size()) {
        return fail("Received packet length is incorrect");
    }

    for (std::size_t i = 0; i < frame.size(); ++i) {
        if (received->buffer->data()[i] != frame[i]) {
            return fail("DMA payload differs from received frame");
        }
    }

    if (ring.consumer_index() != 1 ||
        ring.completed_count() != 3) {
        return fail("Software consumer index did not advance");
    }

    const auto consumed = ring.descriptor_info(0);

    if (!consumed.has_value() ||
        consumed->state != RxDescriptorState::FREE ||
        consumed->owner != RxDescriptorOwner::SOFTWARE ||
        consumed->dma_address != 0 ||
        consumed->received_length != 0) {
        return fail("Consumed descriptor was not cleared correctly");
    }

    if (ring.post()) {
        return fail("Posting succeeded while software held all pool buffers");
    }

    received.reset();

    if (pool.available_count() != 1) {
        return fail("Releasing received packet did not recycle its buffer");
    }

    if (!ring.post()) {
        return fail("Unable to repost descriptor after buffer recycling");
    }

    if (ring.producer_index() != 1 ||
        ring.posted_count() != 1) {
        return fail("Producer wraparound or repost state is incorrect");
    }

    metadata.packet_id = 200;

    if (!ring.simulate_dma_receive(frame, metadata)) {
        return fail("DMA receive failed after descriptor repost");
    }

    for (std::size_t expected_id = 101; expected_id <= 103; ++expected_id) {
        auto packet = ring.poll();

        if (!packet.has_value() ||
            packet->metadata.packet_id != expected_id) {
            return fail("Completion order is incorrect");
        }
    }

    auto wrapped_packet = ring.poll();

    if (!wrapped_packet.has_value() ||
        wrapped_packet->metadata.packet_id != 200) {
        return fail("Wrapped descriptor completion was not delivered");
    }

    wrapped_packet.reset();

    if (ring.completed_count() != 0 ||
        ring.posted_count() != 0 ||
        pool.in_use_count() != 0 ||
        pool.available_count() != POOL_SIZE) {
        return fail("Buffers were not fully recycled after polling");
    }

    if (ring.post_all() != RING_SIZE) {
        return fail("Unable to repost descriptors for oversized-frame test");
    }

    std::vector<std::uint8_t> oversized_frame(BUFFER_CAPACITY, 0xAB);

    if (!ring.simulate_dma_receive(oversized_frame, metadata)) {
        return fail("Oversized frame did not generate an error completion");
    }

    if (ring.posted_count() != RING_SIZE - 1 ||
        ring.completed_count() != 1 ||
        ring.completion_error_count() != 1) {
        return fail("Oversized frame did not produce the expected error completion");
    }

    auto oversized_completion = ring.poll();

    if (!oversized_completion) {
        return fail("Unable to poll oversized-frame completion");
    }

    if (oversized_completion->completion_status !=
            RxCompletionStatus::BUFFER_TOO_SMALL ||
        oversized_completion->received_length != 0) {
        return fail("Oversized frame completion status or length is incorrect");
    }

    oversized_completion.reset();

    if (ring.completed_count() != 0 ||
        pool.in_use_count() != RING_SIZE - 1 ||
        pool.available_count() != 1) {
        return fail("Oversized-frame buffer was not correctly reclaimed");
    }

    if (!ring.post()) {
        return fail("Unable to repost descriptor after error recovery");
    }

    ring.reset();

    if (ring.posted_count() != 0 ||
        ring.completed_count() != 0 ||
        pool.in_use_count() != 0 ||
        pool.available_count() != POOL_SIZE ||
        ring.producer_index() != 0 ||
        ring.device_index() != 0 ||
        ring.consumer_index() != 0) {
        return fail("Ring reset did not restore initial state");
    }

    if (ring.posted_count() != 0 ||
        ring.completed_count() != 0 ||
        pool.in_use_count() != 0 ||
        pool.available_count() != POOL_SIZE ||
        ring.producer_index() != 0 ||
        ring.device_index() != 0 ||
        ring.consumer_index() != 0) {
        return fail("Ring reset did not restore initial state");
    }

    return {
        TestStatus::PASS,
        name(),
        "Descriptor posting, DMA writes, completion, lease transfer, exhaustion, wraparound and reset validated"
    };
}
