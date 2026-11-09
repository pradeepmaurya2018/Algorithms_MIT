#pragma once

#include "hardware/packet_buffer/buffer_pool.hpp"
#include "hardware/packet_io/packet.hpp"
#include "hardware/rx_descriptor_ring/rx_descriptor_ring.hpp"
#include "hardware/tx_descriptor_ring/tx_descriptor_ring.hpp"
#include "ipu/port_manager.hpp"

#include <cstddef>
#include <cstdint>
#include <deque>
#include <optional>

class DatapathPipeline;

class PacketIoEngine {
public:
    explicit PacketIoEngine(
        PortManager& ports,
        std::size_t buffer_count = 256,
        std::size_t buffer_capacity = 9874
    );

    bool set_loopback(std::uint32_t port_id, bool enabled);

    bool inject_packet(
        std::uint32_t ingress_port,
        const Packet& packet
    );

    std::size_t process_packets(std::size_t budget);

    std::size_t process_packets(
        std::size_t budget,
        DatapathPipeline& pipeline
    );

    std::optional<Packet> capture_packet(std::uint32_t egress_port);

    std::size_t pending_ingress() const noexcept;
    std::size_t pending_egress() const noexcept;

    std::size_t buffers_available() const noexcept;
    std::size_t buffers_in_use() const noexcept;

    std::size_t rx_posted_descriptors() const noexcept;
    std::size_t rx_completed_descriptors() const noexcept;

    std::size_t tx_posted_descriptors() const noexcept;
    std::size_t tx_completed_descriptors() const noexcept;

    // Simulate the device consuming up to budget posted TX descriptors.
    std::size_t service_tx(std::size_t budget);

    // Reclaim up to budget TX completions and publish captured frames.
    std::size_t reclaim_tx(std::size_t budget);

    void reset() noexcept;

private:
    struct EgressEntry {
        std::uint32_t port_id;
        Packet packet;
    };

    PortManager& ports_;

    // Declaration order is important: rings reference this pool.
    PacketBufferPool buffer_pool_;
    RxDescriptorRing rx_ring_;
    TxDescriptorRing tx_ring_;

    std::deque<EgressEntry> egress_queue_;
    std::deque<std::uint32_t> loopback_ports_;

    std::uint64_t next_packet_id_{1};

    bool is_loopback_enabled(std::uint32_t port_id) const;

    Packet materialize_legacy_packet(
        const RxPacket& received,
        std::uint32_t egress_port = 0
    ) const;

    bool transmit_packet(const Packet& packet);
};
