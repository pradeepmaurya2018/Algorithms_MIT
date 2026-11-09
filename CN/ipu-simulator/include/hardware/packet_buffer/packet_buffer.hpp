#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

class PacketBuffer {
public:
    explicit PacketBuffer(
        std::size_t capacity,
        std::size_t headroom = 128
    );

    PacketBuffer(const PacketBuffer&) = delete;
    PacketBuffer& operator=(const PacketBuffer&) = delete;

    PacketBuffer(PacketBuffer&&) noexcept = default;
    PacketBuffer& operator=(PacketBuffer&&) noexcept = default;

    std::uint8_t* data() noexcept;
    const std::uint8_t* data() const noexcept;

    std::size_t size() const noexcept;
    std::size_t capacity() const noexcept;
    std::size_t headroom() const noexcept;
    std::size_t tailroom() const noexcept;
    std::size_t initial_headroom() const noexcept;

    bool append(const std::uint8_t* src, std::size_t len) noexcept;
    bool prepend(const std::uint8_t* src, std::size_t len) noexcept;
    bool remove_prefix(std::size_t len) noexcept;
    bool trim(std::size_t new_size) noexcept;

    void reset() noexcept;
    bool empty() const noexcept;

private:
    std::vector<std::uint8_t> storage_;
    std::size_t initial_headroom_{0};
    std::size_t data_offset_{0};
    std::size_t data_size_{0};
};
