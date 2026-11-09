#include "hardware/packet_buffer/buffer_pool.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>

void test_initial_pool_state()
{
    PacketBufferPool pool(4, 2048, 128);

    assert(pool.total_count() == 4);
    assert(pool.available_count() == 4);
    assert(pool.in_use_count() == 0);
    assert(pool.buffer_capacity() == 2048);
}

void test_acquire()
{
    PacketBufferPool pool(2, 2048, 128);

    auto first = pool.acquire();

    assert(first != nullptr);
    assert(pool.available_count() == 1);
    assert(pool.in_use_count() == 1);

    auto second = pool.acquire();

    assert(second != nullptr);
    assert(pool.available_count() == 0);
    assert(pool.in_use_count() == 2);
}

void test_pool_exhaustion()
{
    PacketBufferPool pool(1, 2048, 128);

    auto first = pool.acquire();
    auto second = pool.acquire();

    assert(first != nullptr);
    assert(second == nullptr);
    assert(pool.available_count() == 0);
}

void test_release_and_reuse()
{
    PacketBufferPool pool(1, 2048, 128);

    auto buffer = pool.acquire();

    assert(buffer != nullptr);

    const std::array<std::uint8_t, 4> data{
        0x11, 0x22, 0x33, 0x44
    };

    assert(buffer->append(data.data(), data.size()));
    assert(buffer->size() == 4);

    assert(pool.release(buffer));

    assert(buffer == nullptr);
    assert(pool.available_count() == 1);
    assert(pool.in_use_count() == 0);

    auto reused = pool.acquire();

    assert(reused != nullptr);
    assert(reused->empty());
    assert(reused->size() == 0);
    assert(reused->headroom() == 128);
    assert(reused->tailroom() == 1920);
}

void test_release_after_prepend_and_trim()
{
    PacketBufferPool pool(1, 64, 16);

    auto buffer = pool.acquire();

    const std::array<std::uint8_t, 8> data{};
    assert(buffer->append(data.data(), data.size()));

    const std::array<std::uint8_t, 4> header{};
    assert(buffer->prepend(header.data(), header.size()));
    assert(buffer->trim(6));

    assert(buffer->headroom() == 12);
    assert(buffer->size() == 6);

    assert(pool.release(buffer));

    auto reused = pool.acquire();

    assert(reused->headroom() == 16);
    assert(reused->size() == 0);
    assert(reused->tailroom() == 48);
}

void test_invalid_release()
{
    PacketBufferPool pool(1, 2048, 128);

    auto foreign_buffer =
        std::make_unique<PacketBuffer>(1024, 64);

    assert(!pool.release(foreign_buffer));

    // Failed release must not consume caller ownership.
    assert(foreign_buffer != nullptr);
    assert(pool.available_count() == 1);
}

void test_null_release()
{
    PacketBufferPool pool(1, 2048, 128);

    std::unique_ptr<PacketBuffer> buffer;

    assert(!pool.release(buffer));
    assert(pool.available_count() == 1);
}

void test_zero_sized_pool()
{
    PacketBufferPool pool(0, 2048, 128);

    assert(pool.total_count() == 0);
    assert(pool.available_count() == 0);
    assert(pool.in_use_count() == 0);
    assert(pool.acquire() == nullptr);
}

int main()
{
    test_initial_pool_state();
    test_acquire();
    test_pool_exhaustion();
    test_release_and_reuse();
    test_release_after_prepend_and_trim();
    test_invalid_release();
    test_null_release();
    test_zero_sized_pool();

    std::cout << "All PacketBufferPool tests passed\n";
    return 0;
}
