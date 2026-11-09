#include "hardware/packet_buffer/packet.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <utility>

int main()
{
    using ipu::buffer::BufferedPacket;

    constexpr std::size_t POOL_SIZE = 2;
    constexpr std::size_t CAPACITY = 2048;
    constexpr std::size_t HEADROOM = 128;

    PacketBufferPool pool(POOL_SIZE, CAPACITY, HEADROOM);

    assert(pool.available_count() == POOL_SIZE);

    auto lease = pool.acquire_lease();
    assert(lease);

    PacketMetadata metadata;
    metadata.packet_id = 100;
    metadata.ingress_port = 1;
    metadata.ether_type = 0x0800;
    metadata.is_ipv4 = true;

    BufferedPacket packet(std::move(lease), metadata);

    assert(packet.valid());
    assert(pool.in_use_count() == 1);
    assert(packet.metadata().packet_id == 100);
    assert(packet.metadata().ingress_port == 1);
    assert(packet.metadata().is_ipv4);

    const std::uint8_t bytes[] = {
        0xDE, 0xAD, 0xBE, 0xEF
    };

    assert(packet.buffer().append(bytes, sizeof(bytes)));
    assert(packet.buffer().size() == sizeof(bytes));

    // Ownership transfer must leave the original packet invalid.
    BufferedPacket next_stage(std::move(packet));

    assert(!packet.valid());
    assert(next_stage.valid());
    assert(next_stage.buffer().size() == sizeof(bytes));
    assert(next_stage.metadata().packet_id == 100);

    // The buffer returns to the pool when the final owner is destroyed.
    {
        BufferedPacket final_stage(std::move(next_stage));
        assert(final_stage.valid());
        assert(pool.in_use_count() == 1);
    }

    assert(!next_stage.valid());
    assert(pool.available_count() == POOL_SIZE);
    assert(pool.in_use_count() == 0);

    // A recycled buffer must be reset.
    auto recycled = pool.acquire_lease();
    assert(recycled);
    assert(recycled->empty());
    assert(recycled->headroom() == HEADROOM);

    recycled.reset();

    assert(pool.available_count() == POOL_SIZE);

    std::cout << "All BufferedPacket tests passed\n";
    return 0;
}
