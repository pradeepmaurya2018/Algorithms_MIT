#pragma once

#include "hardware/packet_buffer/packet_buffer.hpp"

#include <cstddef>
#include <memory>
#include <vector>

struct PacketBufferPoolState;

class PacketBufferLease;

class PacketBufferPool {
public:
    PacketBufferPool(
        std::size_t pool_size,
        std::size_t buffer_capacity,
        std::size_t headroom = 128
    );

    PacketBufferPool(const PacketBufferPool&) = delete;
    PacketBufferPool& operator=(const PacketBufferPool&) = delete;

    std::unique_ptr<PacketBuffer> acquire() noexcept;
    bool release(std::unique_ptr<PacketBuffer>& buffer) noexcept;

    PacketBufferLease acquire_lease() noexcept;

    std::size_t total_count() const noexcept;
    std::size_t available_count() const noexcept;
    std::size_t in_use_count() const noexcept;
    std::size_t buffer_capacity() const noexcept;

private:
    friend class PacketBufferLease;

    std::shared_ptr<PacketBufferPoolState> state_;
};

class PacketBufferLease {
public:
    PacketBufferLease() noexcept = default;

    PacketBufferLease(const PacketBufferLease&) = delete;
    PacketBufferLease& operator=(const PacketBufferLease&) = delete;

    PacketBufferLease(PacketBufferLease&& other) noexcept;
    PacketBufferLease& operator=(PacketBufferLease&& other) noexcept;

    ~PacketBufferLease();

    PacketBuffer* get() noexcept;
    const PacketBuffer* get() const noexcept;

    PacketBuffer& operator*() noexcept;
    const PacketBuffer& operator*() const noexcept;

    PacketBuffer* operator->() noexcept;
    const PacketBuffer* operator->() const noexcept;

    explicit operator bool() const noexcept;

    void reset() noexcept;

private:
    friend class PacketBufferPool;

    PacketBufferLease(
        std::shared_ptr<PacketBufferPoolState> state,
        std::unique_ptr<PacketBuffer> buffer
    ) noexcept;

    std::shared_ptr<PacketBufferPoolState> state_;
    std::unique_ptr<PacketBuffer> buffer_;
};
