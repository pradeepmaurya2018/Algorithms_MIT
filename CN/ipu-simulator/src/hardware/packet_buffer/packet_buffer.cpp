#include "hardware/packet_buffer/packet_buffer.hpp"

#include <cstring>
#include <stdexcept>

PacketBuffer::PacketBuffer(
    std::size_t capacity,
    std::size_t headroom
)
    : storage_(capacity),
      initial_headroom_(headroom),
      data_offset_(headroom)
{
    if (headroom > capacity) {
        throw std::invalid_argument(
            "PacketBuffer headroom exceeds capacity"
        );
    }
}

std::uint8_t* PacketBuffer::data() noexcept
{
    if (storage_.empty()) {
        return nullptr;
    }

    return storage_.data() + data_offset_;
}

const std::uint8_t* PacketBuffer::data() const noexcept
{
    if (storage_.empty()) {
        return nullptr;
    }

    return storage_.data() + data_offset_;
}

std::size_t PacketBuffer::size() const noexcept
{
    return data_size_;
}

std::size_t PacketBuffer::capacity() const noexcept
{
    return storage_.size();
}

std::size_t PacketBuffer::headroom() const noexcept
{
    return data_offset_;
}

std::size_t PacketBuffer::tailroom() const noexcept
{
    return storage_.size() - data_offset_ - data_size_;
}

std::size_t PacketBuffer::initial_headroom() const noexcept
{
    return initial_headroom_;
}

bool PacketBuffer::empty() const noexcept
{
    return data_size_ == 0;
}

void PacketBuffer::reset() noexcept
{
    data_offset_ = initial_headroom_;
    data_size_ = 0;
}

bool PacketBuffer::append(
    const std::uint8_t* src,
    std::size_t len
) noexcept
{
    if (len == 0) {
        return true;
    }

    if (src == nullptr || len > tailroom()) {
        return false;
    }

    std::memmove(data() + data_size_, src, len);
    data_size_ += len;
    return true;
}

bool PacketBuffer::prepend(
    const std::uint8_t* src,
    std::size_t len
) noexcept
{
    if (len == 0) {
        return true;
    }

    if (src == nullptr || len > headroom()) {
        return false;
    }

    const std::size_t new_offset = data_offset_ - len;

    std::memmove(storage_.data() + new_offset, src, len);

    data_offset_ = new_offset;
    data_size_ += len;
    return true;
}

bool PacketBuffer::remove_prefix(std::size_t len) noexcept
{
    if (len > data_size_) {
        return false;
    }

    data_offset_ += len;
    data_size_ -= len;
    return true;
}

bool PacketBuffer::trim(std::size_t new_size) noexcept
{
    if (new_size > data_size_) {
        return false;
    }

    data_size_ = new_size;
    return true;
}
