#pragma once

#include "hardware/packet_buffer/buffer_pool.hpp"
#include "hardware/packet_buffer/packet_metadata.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <utility>
#include <vector>

enum class RxDescriptorState {
    FREE,
    POSTED,
    COMPLETED
};

enum class RxDescriptorOwner {
    SOFTWARE,
    DEVICE
};

enum class RxCompletionStatus {
    NONE,
    SUCCESS,
    BUFFER_TOO_SMALL
};

// Read-only representation of the descriptor fields.
// dma_address is a synthetic IOVA, not a physical address.
struct RxDescriptorSnapshot {
    RxDescriptorState state{RxDescriptorState::FREE};
    RxDescriptorOwner owner{RxDescriptorOwner::SOFTWARE};
    RxCompletionStatus completion_status{RxCompletionStatus::NONE};

    std::uint64_t dma_address{0};
    std::size_t buffer_capacity{0};
    std::size_t received_length{0};
};

struct RxPacket {
    PacketBufferLease buffer;
    PacketMetadata metadata;
    RxCompletionStatus completion_status{RxCompletionStatus::NONE};
    std::size_t received_length{0};

    RxPacket(
        PacketBufferLease&& lease,
        PacketMetadata packet_metadata,
        RxCompletionStatus status,
        std::size_t length
    ) noexcept
        : buffer(std::move(lease)),
          metadata(packet_metadata),
          completion_status(status),
          received_length(length)
    {
    }

    RxPacket(const RxPacket&) = delete;
    RxPacket& operator=(const RxPacket&) = delete;

    RxPacket(RxPacket&&) noexcept = default;
    RxPacket& operator=(RxPacket&&) noexcept = default;
};

class RxDescriptorRing {
public:
    RxDescriptorRing(PacketBufferPool& pool, std::size_t ring_size);

    RxDescriptorRing(const RxDescriptorRing&) = delete;
    RxDescriptorRing& operator=(const RxDescriptorRing&) = delete;

    bool post();
    std::size_t post_all();

    bool simulate_dma_receive(
        std::span<const std::uint8_t> frame,
        const PacketMetadata& metadata
    ) noexcept;

    std::optional<RxPacket> poll() noexcept;

    void reset() noexcept;

    std::size_t ring_size() const noexcept;
    std::size_t producer_index() const noexcept;
    std::size_t device_index() const noexcept;
    std::size_t consumer_index() const noexcept;

    std::size_t posted_count() const noexcept;
    std::size_t completed_count() const noexcept;
    std::size_t completion_error_count() const noexcept;

    std::optional<RxDescriptorState> descriptor_state(
        std::size_t index
    ) const noexcept;

    std::optional<RxDescriptorSnapshot> descriptor_info(
        std::size_t index
    ) const noexcept;

private:
    struct Descriptor {
        RxDescriptorState state{RxDescriptorState::FREE};
        RxDescriptorOwner owner{RxDescriptorOwner::SOFTWARE};
        RxCompletionStatus completion_status{RxCompletionStatus::NONE};

        std::uint64_t dma_address{0};
        std::size_t buffer_capacity{0};
        std::size_t received_length{0};

        PacketBufferLease buffer;
        PacketMetadata metadata;
    };

    PacketBufferPool& pool_;
    std::vector<Descriptor> descriptors_;

    std::size_t producer_index_{0};
    std::size_t device_index_{0};
    std::size_t consumer_index_{0};

    // Synthetic address allocator for simulator-only DMA addresses.
    std::uint64_t next_dma_address_{0x10000000ULL};
    std::size_t completion_error_count_{0};
};
