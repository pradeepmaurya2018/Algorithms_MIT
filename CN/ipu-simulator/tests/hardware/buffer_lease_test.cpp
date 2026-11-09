#include "hardware/packet_buffer/buffer_pool.hpp"

#include <array>
#include <cassert>
#include <iostream>
#include <type_traits>
#include <utility>

static_assert(!std::is_copy_constructible_v<PacketBufferLease>);
static_assert(std::is_move_constructible_v<PacketBufferLease>);
static_assert(std::is_move_assignable_v<PacketBufferLease>);

void test_acquire_lease()
{
    PacketBufferPool pool(2, 2048, 128);

    {
        auto lease = pool.acquire_lease();

        assert(lease);
        assert(pool.available_count() == 1);
        assert(pool.in_use_count() == 1);
        assert(lease->headroom() == 128);
    }

    // Destruction returns the buffer.
    assert(pool.available_count() == 2);
    assert(pool.in_use_count() == 0);
}

void test_lease_recycles_modified_buffer()
{
    PacketBufferPool pool(1, 128, 32);

    {
        auto lease = pool.acquire_lease();

        const std::array<std::uint8_t, 8> payload{
            1, 2, 3, 4, 5, 6, 7, 8
        };

        assert(lease->append(payload.data(), payload.size()));
        assert(lease->remove_prefix(3));
        assert(lease->size() == 5);
    }

    assert(pool.available_count() == 1);

    auto reused = pool.acquire_lease();

    assert(reused);
    assert(reused->empty());
    assert(reused->size() == 0);
    assert(reused->headroom() == 32);
    assert(reused->tailroom() == 96);
}

void test_move_construction()
{
    PacketBufferPool pool(1, 512, 64);

    auto first = pool.acquire_lease();
    assert(first);

    auto second = std::move(first);

    assert(!first);
    assert(second);
    assert(pool.in_use_count() == 1);

    second.reset();

    assert(!second);
    assert(pool.available_count() == 1);
}

void test_move_assignment()
{
    PacketBufferPool pool(2, 512, 64);

    auto first = pool.acquire_lease();
    auto second = pool.acquire_lease();

    assert(first);
    assert(second);
    assert(pool.in_use_count() == 2);

    // The old buffer owned by first must be recycled.
    first = std::move(second);

    assert(first);
    assert(!second);
    assert(pool.available_count() == 1);
    assert(pool.in_use_count() == 1);
}

void test_empty_pool_lease()
{
    PacketBufferPool pool(0, 512, 64);

    auto lease = pool.acquire_lease();

    assert(!lease);
}

void test_explicit_reset()
{
    PacketBufferPool pool(1, 512, 64);

    auto lease = pool.acquire_lease();

    assert(lease);
    assert(pool.in_use_count() == 1);

    lease.reset();

    assert(!lease);
    assert(pool.available_count() == 1);
}

void test_lease_outlives_pool()
{
    PacketBufferLease lease;

    {
        PacketBufferPool pool(1, 512, 64);
        lease = pool.acquire_lease();

        assert(lease);
    }

    // The shared pool state remains alive through the lease.
    assert(lease);
    assert(lease->capacity() == 512);

    lease.reset();
    assert(!lease);
}

int main()
{
    test_acquire_lease();
    test_lease_recycles_modified_buffer();
    test_move_construction();
    test_move_assignment();
    test_empty_pool_lease();
    test_explicit_reset();
    test_lease_outlives_pool();

    std::cout << "All PacketBufferLease tests passed\n";
    return 0;
}
