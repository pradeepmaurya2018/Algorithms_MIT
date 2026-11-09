#include "hardware/rx_descriptor_ring/rx_descriptor_ring.hpp"

#include <stdexcept>
#include <utility>

RxDescriptorRing::RxDescriptorRing(
    PacketBufferPool& pool,
    std::size_t ring_size
)
    : pool_(pool),
      descriptors_(ring_size)
{
    if (ring_size == 0) {
        throw std::invalid_argument("RX ring size cannot be zero");
    }
}

bool RxDescriptorRing::post()
{
    Descriptor& descriptor = descriptors_[producer_index_];

    if (descriptor.state != RxDescriptorState::FREE) {
        return false;
    }

    PacketBufferLease lease = pool_.acquire_lease();

    if (!lease) {
        return false;
    }

    descriptor.buffer = std::move(lease);
    descriptor.metadata = {};

    descriptor.dma_address = next_dma_address_;
    next_dma_address_ += 0x1000ULL;

    descriptor.buffer_capacity = descriptor.buffer->tailroom();
    descriptor.received_length = 0;
    descriptor.completion_status = RxCompletionStatus::NONE;

    descriptor.owner = RxDescriptorOwner::DEVICE;
    descriptor.state = RxDescriptorState::POSTED;

    producer_index_ = (producer_index_ + 1) % descriptors_.size();

    return true;
}

std::size_t RxDescriptorRing::post_all()
{
    std::size_t posted = 0;

    for (std::size_t i = 0; i < descriptors_.size(); ++i) {
        if (!post()) {
            break;
        }

        ++posted;
    }

    return posted;
}

bool RxDescriptorRing::simulate_dma_receive(
    std::span<const std::uint8_t> frame,
    const PacketMetadata& metadata
) noexcept
{
    if (frame.empty()) {
        return false;
    }

    Descriptor& descriptor = descriptors_[device_index_];

    if (descriptor.state != RxDescriptorState::POSTED) {
        return false;
    }

    descriptor.metadata = metadata;

    if (!descriptor.buffer->append(frame.data(), frame.size())) {
        // DMA could not fit the frame into the posted buffer.
        // Publish an error completion so software can reclaim the buffer.
        descriptor.received_length = 0;
        descriptor.completion_status = RxCompletionStatus::BUFFER_TOO_SMALL;
        ++completion_error_count_;
    } else {
        descriptor.received_length = frame.size();
        descriptor.completion_status = RxCompletionStatus::SUCCESS;
    }

    // Publish completion and return ownership to software.
    descriptor.owner = RxDescriptorOwner::SOFTWARE;
    descriptor.state = RxDescriptorState::COMPLETED;

    device_index_ = (device_index_ + 1) % descriptors_.size();

    return true;
}

std::optional<RxPacket> RxDescriptorRing::poll() noexcept
{
    Descriptor& descriptor = descriptors_[consumer_index_];

    if (descriptor.state != RxDescriptorState::COMPLETED ||
        descriptor.owner != RxDescriptorOwner::SOFTWARE ||
        descriptor.completion_status == RxCompletionStatus::NONE) {
        return std::nullopt;
    }

    RxPacket packet(
        std::move(descriptor.buffer),
        descriptor.metadata,
        descriptor.completion_status,
        descriptor.received_length
    );

    descriptor.metadata = {};
    descriptor.dma_address = 0;
    descriptor.buffer_capacity = 0;
    descriptor.received_length = 0;
    descriptor.completion_status = RxCompletionStatus::NONE;

    descriptor.owner = RxDescriptorOwner::SOFTWARE;
    descriptor.state = RxDescriptorState::FREE;

    consumer_index_ = (consumer_index_ + 1) % descriptors_.size();

    return packet;
}

void RxDescriptorRing::reset() noexcept
{
    for (Descriptor& descriptor : descriptors_) {
        descriptor.buffer.reset();
        descriptor.metadata = {};

        descriptor.dma_address = 0;
        descriptor.buffer_capacity = 0;
        descriptor.received_length = 0;
        descriptor.completion_status = RxCompletionStatus::NONE;

        descriptor.owner = RxDescriptorOwner::SOFTWARE;
        descriptor.state = RxDescriptorState::FREE;
    }

    producer_index_ = 0;
    device_index_ = 0;
    consumer_index_ = 0;
    completion_error_count_ = 0;
}

std::size_t RxDescriptorRing::ring_size() const noexcept
{
    return descriptors_.size();
}

std::size_t RxDescriptorRing::producer_index() const noexcept
{
    return producer_index_;
}

std::size_t RxDescriptorRing::device_index() const noexcept
{
    return device_index_;
}

std::size_t RxDescriptorRing::consumer_index() const noexcept
{
    return consumer_index_;
}

std::size_t RxDescriptorRing::posted_count() const noexcept
{
    std::size_t count = 0;

    for (const Descriptor& descriptor : descriptors_) {
        if (descriptor.state == RxDescriptorState::POSTED) {
            ++count;
        }
    }

    return count;
}

std::size_t RxDescriptorRing::completed_count() const noexcept
{
    std::size_t count = 0;

    for (const Descriptor& descriptor : descriptors_) {
        if (descriptor.state == RxDescriptorState::COMPLETED) {
            ++count;
        }
    }

    return count;
}

std::size_t RxDescriptorRing::completion_error_count() const noexcept
{
    return completion_error_count_;
}

std::optional<RxDescriptorState> RxDescriptorRing::descriptor_state(
    std::size_t index
) const noexcept
{
    if (index >= descriptors_.size()) {
        return std::nullopt;
    }

    return descriptors_[index].state;
}

std::optional<RxDescriptorSnapshot> RxDescriptorRing::descriptor_info(
    std::size_t index
) const noexcept
{
    if (index >= descriptors_.size()) {
        return std::nullopt;
    }

    const Descriptor& descriptor = descriptors_[index];

    return RxDescriptorSnapshot{
        descriptor.state,
        descriptor.owner,
        descriptor.completion_status,
        descriptor.dma_address,
        descriptor.buffer_capacity,
        descriptor.received_length
    };
}
