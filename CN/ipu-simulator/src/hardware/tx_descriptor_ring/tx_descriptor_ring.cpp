#include "hardware/tx_descriptor_ring/tx_descriptor_ring.hpp"

#include <stdexcept>
#include <utility>

TxDescriptorRing::TxDescriptorRing(std::size_t ring_size)
    : descriptors_(ring_size)
{
    if (ring_size == 0) {
        throw std::invalid_argument("TX ring size cannot be zero");
    }
}

bool TxDescriptorRing::submit(
    PacketBufferLease&& buffer,
    const PacketMetadata& metadata
)
{
    if (!buffer || buffer->empty()) {
        return false;
    }

    Descriptor& descriptor = descriptors_[producer_index_];

    if (descriptor.state != TxDescriptorState::FREE) {
        return false;
    }

    descriptor.packet_length = buffer->size();
    descriptor.metadata = metadata;
    descriptor.buffer = std::move(buffer);

    descriptor.dma_address = next_dma_address_;
    next_dma_address_ += 0x1000ULL;

    descriptor.completion_status = TxCompletionStatus::NONE;
    descriptor.owner = TxDescriptorOwner::DEVICE;
    descriptor.state = TxDescriptorState::POSTED;

    producer_index_ = (producer_index_ + 1) % descriptors_.size();

    return true;
}

bool TxDescriptorRing::simulate_dma_transmit()
{
    Descriptor& descriptor = descriptors_[device_index_];

    if (descriptor.state != TxDescriptorState::POSTED ||
        descriptor.owner != TxDescriptorOwner::DEVICE ||
        !descriptor.buffer) {
        return false;
    }

    // The simulated device reads the packet payload.
    const std::uint8_t* begin = descriptor.buffer->data();
    const std::size_t length = descriptor.buffer->size();

    std::vector<std::uint8_t> frame(begin, begin + length);

    transmitted_frames_.push_back(std::move(frame));

    descriptor.completion_status = TxCompletionStatus::SUCCESS;
    descriptor.owner = TxDescriptorOwner::SOFTWARE;
    descriptor.state = TxDescriptorState::COMPLETED;

    device_index_ = (device_index_ + 1) % descriptors_.size();

    return true;
}

std::optional<TxCompletion> TxDescriptorRing::poll() noexcept
{
    Descriptor& descriptor = descriptors_[consumer_index_];

    if (descriptor.state != TxDescriptorState::COMPLETED ||
        descriptor.owner != TxDescriptorOwner::SOFTWARE ||
        descriptor.completion_status == TxCompletionStatus::NONE) {
        return std::nullopt;
    }

    TxCompletion completion(
        std::move(descriptor.buffer),
        descriptor.metadata,
        descriptor.completion_status
    );

    descriptor.metadata = {};
    descriptor.dma_address = 0;
    descriptor.packet_length = 0;
    descriptor.completion_status = TxCompletionStatus::NONE;
    descriptor.owner = TxDescriptorOwner::SOFTWARE;
    descriptor.state = TxDescriptorState::FREE;

    consumer_index_ = (consumer_index_ + 1) % descriptors_.size();

    return completion;
}

std::optional<std::vector<std::uint8_t>>
TxDescriptorRing::take_transmitted_frame()
{
    if (transmitted_read_index_ >= transmitted_frames_.size()) {
        return std::nullopt;
    }

    std::vector<std::uint8_t> frame =
        std::move(transmitted_frames_[transmitted_read_index_]);

    ++transmitted_read_index_;

    if (transmitted_read_index_ == transmitted_frames_.size()) {
        transmitted_frames_.clear();
        transmitted_read_index_ = 0;
    }

    return frame;
}

void TxDescriptorRing::reset() noexcept
{
    for (Descriptor& descriptor : descriptors_) {
        descriptor.buffer.reset();
        descriptor.metadata = {};
        descriptor.dma_address = 0;
        descriptor.packet_length = 0;
        descriptor.completion_status = TxCompletionStatus::NONE;
        descriptor.owner = TxDescriptorOwner::SOFTWARE;
        descriptor.state = TxDescriptorState::FREE;
    }

    transmitted_frames_.clear();
    transmitted_read_index_ = 0;

    producer_index_ = 0;
    device_index_ = 0;
    consumer_index_ = 0;
}

std::size_t TxDescriptorRing::ring_size() const noexcept
{
    return descriptors_.size();
}

std::size_t TxDescriptorRing::producer_index() const noexcept
{
    return producer_index_;
}

std::size_t TxDescriptorRing::device_index() const noexcept
{
    return device_index_;
}

std::size_t TxDescriptorRing::consumer_index() const noexcept
{
    return consumer_index_;
}

std::size_t TxDescriptorRing::posted_count() const noexcept
{
    std::size_t count = 0;

    for (const Descriptor& descriptor : descriptors_) {
        if (descriptor.state == TxDescriptorState::POSTED) {
            ++count;
        }
    }

    return count;
}

std::size_t TxDescriptorRing::completed_count() const noexcept
{
    std::size_t count = 0;

    for (const Descriptor& descriptor : descriptors_) {
        if (descriptor.state == TxDescriptorState::COMPLETED) {
            ++count;
        }
    }

    return count;
}

std::size_t TxDescriptorRing::transmitted_count() const noexcept
{
    return transmitted_frames_.size() - transmitted_read_index_;
}

std::optional<TxDescriptorState> TxDescriptorRing::descriptor_state(
    std::size_t index
) const noexcept
{
    if (index >= descriptors_.size()) {
        return std::nullopt;
    }

    return descriptors_[index].state;
}
