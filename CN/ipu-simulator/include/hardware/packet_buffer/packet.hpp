#pragma once

#include "hardware/packet_buffer/buffer_pool.hpp"
#include "hardware/packet_buffer/packet_metadata.hpp"

#include <utility>

namespace ipu::buffer {

class BufferedPacket {
public:
    explicit BufferedPacket(
        PacketBufferLease&& lease,
        PacketMetadata metadata = {}
    ) noexcept
        : lease_(std::move(lease)),
          metadata_(metadata)
    {
    }

    BufferedPacket(const BufferedPacket&) = delete;
    BufferedPacket& operator=(const BufferedPacket&) = delete;

    BufferedPacket(BufferedPacket&&) noexcept = default;
    BufferedPacket& operator=(BufferedPacket&&) noexcept = default;

    bool valid() const noexcept
    {
        return static_cast<bool>(lease_);
    }

    PacketBuffer& buffer() noexcept
    {
        return *lease_;
    }

    const PacketBuffer& buffer() const noexcept
    {
        return *lease_;
    }

    PacketMetadata& metadata() noexcept
    {
        return metadata_;
    }

    const PacketMetadata& metadata() const noexcept
    {
        return metadata_;
    }

private:
    PacketBufferLease lease_;
    PacketMetadata metadata_;
};

} // namespace ipu::buffer
