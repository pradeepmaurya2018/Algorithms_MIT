#include "hardware/packet_buffer/packet.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <type_traits>

static_assert(!std::is_copy_constructible_v<Packet>);
static_assert(std::is_move_constructible_v<Packet>);
static_assert(!std::is_copy_constructible_v<PacketBuffer>);
static_assert(std::is_move_constructible_v<PacketBuffer>);

void test_initial_state()
{
    PacketBuffer buffer(2048, 128);

    assert(buffer.capacity() == 2048);
    assert(buffer.size() == 0);
    assert(buffer.headroom() == 128);
    assert(buffer.tailroom() == 1920);
    assert(buffer.empty());
}

void test_append()
{
    PacketBuffer buffer(64, 16);

    const std::array<std::uint8_t, 4> payload{
        0x11, 0x22, 0x33, 0x44
    };

    assert(buffer.append(payload.data(), payload.size()));
    assert(buffer.size() == 4);
    assert(buffer.headroom() == 16);
    assert(buffer.tailroom() == 44);

    for (std::size_t i = 0; i < payload.size(); ++i) {
        assert(buffer.data()[i] == payload[i]);
    }
}

void test_prepend()
{
    PacketBuffer buffer(64, 16);

    const std::array<std::uint8_t, 4> payload{
        0xAA, 0xBB, 0xCC, 0xDD
    };

    const std::array<std::uint8_t, 2> header{
        0x08, 0x00
    };

    assert(buffer.append(payload.data(), payload.size()));

    auto* old_data = buffer.data();

    assert(buffer.prepend(header.data(), header.size()));

    assert(buffer.size() == 6);
    assert(buffer.headroom() == 14);

    // Prepending uses existing storage, not a new allocation.
    assert(buffer.data() == old_data - 2);

    assert(buffer.data()[0] == 0x08);
    assert(buffer.data()[1] == 0x00);
    assert(buffer.data()[2] == 0xAA);
    assert(buffer.data()[3] == 0xBB);
    assert(buffer.data()[4] == 0xCC);
    assert(buffer.data()[5] == 0xDD);
}

void test_remove_prefix()
{
    PacketBuffer buffer(64, 8);

    const std::array<std::uint8_t, 6> packet{
        1, 2, 3, 4, 5, 6
    };

    assert(buffer.append(packet.data(), packet.size()));
    assert(buffer.remove_prefix(2));

    assert(buffer.size() == 4);
    assert(buffer.headroom() == 10);
    assert(buffer.data()[0] == 3);
    assert(buffer.data()[3] == 6);
}

void test_trim()
{
    PacketBuffer buffer(64, 8);

    const std::array<std::uint8_t, 6> packet{
        1, 2, 3, 4, 5, 6
    };

    assert(buffer.append(packet.data(), packet.size()));
    assert(buffer.trim(3));

    assert(buffer.size() == 3);
    assert(buffer.data()[0] == 1);
    assert(buffer.data()[2] == 3);
}

void test_capacity_limits()
{
    PacketBuffer buffer(16, 4);

    const std::array<std::uint8_t, 12> bytes{};

    assert(buffer.append(bytes.data(), bytes.size()));
    assert(buffer.tailroom() == 0);

    assert(!buffer.append(bytes.data(), 1));
    assert(!buffer.prepend(bytes.data(), 5));

    assert(buffer.size() == 12);
    assert(buffer.headroom() == 4);
    assert(buffer.tailroom() == 0);
}

void test_invalid_operations()
{
    PacketBuffer buffer(32, 8);

    const std::array<std::uint8_t, 4> bytes{1, 2, 3, 4};

    assert(!buffer.append(nullptr, 4));
    assert(!buffer.prepend(nullptr, 2));
    assert(!buffer.remove_prefix(1));
    assert(!buffer.trim(1));

    // Zero-length operations are valid no-ops.
    assert(buffer.append(nullptr, 0));
    assert(buffer.prepend(nullptr, 0));
    assert(buffer.remove_prefix(0));
    assert(buffer.trim(0));

    assert(buffer.append(bytes.data(), bytes.size()));
    assert(!buffer.remove_prefix(5));
    assert(!buffer.trim(5));

    assert(buffer.size() == 4);
}

void test_constructor_validation()
{
    bool threw = false;

    try {
        PacketBuffer invalid(64, 65);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);
}

void test_packet_metadata()
{
    PacketBuffer buffer(2048, 128);

    PacketMetadata metadata{};
    metadata.packet_id = 42;
    metadata.ingress_port = 1;
    metadata.vlan_id = 100;
    metadata.ether_type = 0x0800;
    metadata.is_ipv4 = true;

    Packet packet(std::move(buffer), metadata);

    assert(packet.metadata().packet_id == 42);
    assert(packet.metadata().ingress_port == 1);
    assert(packet.metadata().vlan_id == 100);
    assert(packet.metadata().ether_type == 0x0800);
    assert(packet.metadata().is_ipv4);
}

int main()
{
    test_initial_state();
    test_append();
    test_prepend();
    test_remove_prefix();
    test_trim();
    test_capacity_limits();
    test_invalid_operations();
    test_constructor_validation();
    test_packet_metadata();

    std::cout << "All PacketBuffer tests passed\n";
    return 0;
}
