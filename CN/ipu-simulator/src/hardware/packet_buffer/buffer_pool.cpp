#include "hardware/packet_buffer/buffer_pool.hpp"

#include <stdexcept>
#include <utility>

struct PacketBufferPoolState {
    std::size_t total_count{0};
    std::size_t buffer_capacity{0};
    std::size_t headroom{0};

    std::vector<std::unique_ptr<PacketBuffer>> available;
};

namespace {

bool return_buffer(
    const std::shared_ptr<PacketBufferPoolState>& state,
    std::unique_ptr<PacketBuffer>& buffer
) noexcept
{
    if (!state || !buffer) {
        return false;
    }

    if (buffer->capacity() != state->buffer_capacity ||
        buffer->initial_headroom() != state->headroom) {
        return false;
    }

    if (state->available.size() >= state->total_count) {
        return false;
    }

    buffer->reset();
    state->available.push_back(std::move(buffer));

    return true;
}

} // namespace

PacketBufferPool::PacketBufferPool(
    std::size_t pool_size,
    std::size_t buffer_capacity,
    std::size_t headroom
)
    : state_(std::make_shared<PacketBufferPoolState>())
{
    if (headroom > buffer_capacity) {
        throw std::invalid_argument(
            "Pool headroom exceeds buffer capacity"
        );
    }

    state_->total_count = pool_size;
    state_->buffer_capacity = buffer_capacity;
    state_->headroom = headroom;

    state_->available.reserve(pool_size);

    for (std::size_t i = 0; i < pool_size; ++i) {
        state_->available.push_back(
            std::make_unique<PacketBuffer>(
                buffer_capacity,
                headroom
            )
        );
    }
}

std::unique_ptr<PacketBuffer> PacketBufferPool::acquire() noexcept
{
    if (state_->available.empty()) {
        return nullptr;
    }

    auto buffer = std::move(state_->available.back());
    state_->available.pop_back();

    return buffer;
}

bool PacketBufferPool::release(
    std::unique_ptr<PacketBuffer>& buffer
) noexcept
{
    return return_buffer(state_, buffer);
}

PacketBufferLease PacketBufferPool::acquire_lease() noexcept
{
    return PacketBufferLease(state_, acquire());
}

std::size_t PacketBufferPool::total_count() const noexcept
{
    return state_->total_count;
}

std::size_t PacketBufferPool::available_count() const noexcept
{
    return state_->available.size();
}

std::size_t PacketBufferPool::in_use_count() const noexcept
{
    return state_->total_count - state_->available.size();
}

std::size_t PacketBufferPool::buffer_capacity() const noexcept
{
    return state_->buffer_capacity;
}

PacketBufferLease::PacketBufferLease(
    std::shared_ptr<PacketBufferPoolState> state,
    std::unique_ptr<PacketBuffer> buffer
) noexcept
    : state_(std::move(state)),
      buffer_(std::move(buffer))
{
}

PacketBufferLease::PacketBufferLease(
    PacketBufferLease&& other
) noexcept
    : state_(std::move(other.state_)),
      buffer_(std::move(other.buffer_))
{
}

PacketBufferLease& PacketBufferLease::operator=(
    PacketBufferLease&& other
) noexcept
{
    if (this != &other) {
        reset();

        state_ = std::move(other.state_);
        buffer_ = std::move(other.buffer_);
    }

    return *this;
}

PacketBufferLease::~PacketBufferLease()
{
    reset();
}

PacketBuffer* PacketBufferLease::get() noexcept
{
    return buffer_.get();
}

const PacketBuffer* PacketBufferLease::get() const noexcept
{
    return buffer_.get();
}

PacketBuffer& PacketBufferLease::operator*() noexcept
{
    return *buffer_;
}

const PacketBuffer& PacketBufferLease::operator*() const noexcept
{
    return *buffer_;
}

PacketBuffer* PacketBufferLease::operator->() noexcept
{
    return buffer_.get();
}

const PacketBuffer* PacketBufferLease::operator->() const noexcept
{
    return buffer_.get();
}

PacketBufferLease::operator bool() const noexcept
{
    return buffer_ != nullptr;
}

void PacketBufferLease::reset() noexcept
{
    if (buffer_) {
        return_buffer(state_, buffer_);
    }

    state_.reset();
}
