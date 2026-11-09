#pragma once

#include "hardware/packet_buffer/buffer_pool.hpp"
#include "hardware/packet_buffer/packet_metadata.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>
#include <vector>

enum class TxDescriptorState {
    FREE,
    POSTED,
    COMPLETED
};

enum class TxDescriptorOwner {
    SOFTWARE,
    DEVICE
};

enum class TxCompletionStatus {
    NONE,
    SUCCESS,
    INVALID_BUFFER
};

struct TxCompletion {
    PacketBufferLease buffer;
    PacketMetadata metadata;
    TxCompletionStatus status{TxCompletionStatus::NONE};

    TxCompletion(
        PacketBufferLease&& lease,
        PacketMetadata packet_metadata,
        TxCompletionStatus completion_status
    ) noexcept
        : buffer(std::move(lease)),
          metadata(packet_metadata),
          status(completion_status)
    {
    }

    TxCompletion(const TxCompletion&) = delete;
    TxCompletion& operator=(const TxCompletion&) = delete;

    TxCompletion(TxCompletion&&) noexcept = default;
    TxCompletion& operator=(TxCompletion&&) noexcept = default;
};

class TxDescriptorRing {
public:
    explicit TxDescriptorRing(std::size_t ring_size);

    TxDescriptorRing(const TxDescriptorRing&) = delete;
    TxDescriptorRing& operator=(const TxDescriptorRing&) = delete;

    // Software submits a packet buffer for transmission.
    bool submit(
        PacketBufferLease&& buffer,
        const PacketMetadata& metadata
    );

    // Simulated device reads the submitted buffer and transmits it.
    bool simulate_dma_transmit();

    // Software retrieves a completed transmission.
    std::optional<TxCompletion> poll() noexcept;

    // Retrieve a copy of a transmitted frame for validation.
    std::optional<std::vector<std::uint8_t>> take_transmitted_frame();

    void reset() noexcept;

    std::size_t ring_size() const noexcept;
    std::size_t producer_index() const noexcept;
    std::size_t device_index() const noexcept;
    std::size_t consumer_index() const noexcept;

    std::size_t posted_count() const noexcept;
    std::size_t completed_count() const noexcept;

    std::size_t transmitted_count() const noexcept;

    std::optional<TxDescriptorState> descriptor_state(
        std::size_t index
    ) const noexcept;

private:
    struct Descriptor {
        TxDescriptorState state{TxDescriptorState::FREE};
        TxDescriptorOwner owner{TxDescriptorOwner::SOFTWARE};
        TxCompletionStatus completion_status{TxCompletionStatus::NONE};

        std::uint64_t dma_address{0};
        std::size_t packet_length{0};

        PacketBufferLease buffer;
        PacketMetadata metadata;
    };

    std::vector<Descriptor> descriptors_;

    std::vector<std::vector<std::uint8_t>> transmitted_frames_;
    std::size_t transmitted_read_index_{0};

    std::size_t producer_index_{0};
    std::size_t device_index_{0};
    std::size_t consumer_index_{0};

    std::uint64_t next_dma_address_{0x20000000ULL};
};
