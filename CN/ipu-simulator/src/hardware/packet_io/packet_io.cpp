#include "hardware/packet_io/packet_io.hpp"

#include "hardware/datapath_pipeline/datapath_pipeline.hpp"

#include <algorithm>
#include <span>
#include <utility>

namespace {

constexpr std::size_t RX_BUFFER_HEADROOM = 128;

} // namespace

PacketIoEngine::PacketIoEngine(
    PortManager& ports,
    std::size_t buffer_count,
    std::size_t buffer_capacity
)
    : ports_(ports),
      buffer_pool_(buffer_count, buffer_capacity, RX_BUFFER_HEADROOM),
      rx_ring_(buffer_pool_, buffer_count),
      tx_ring_(buffer_count)
{
}

bool PacketIoEngine::set_loopback(
    std::uint32_t port_id,
    bool enabled
)
{
    if (ports_.get_port(port_id) == nullptr) {
        return false;
    }

    const auto it = std::find(
        loopback_ports_.begin(),
        loopback_ports_.end(),
        port_id
    );

    if (enabled && it == loopback_ports_.end()) {
        loopback_ports_.push_back(port_id);
    } else if (!enabled && it != loopback_ports_.end()) {
        loopback_ports_.erase(it);
    }

    return true;
}

bool PacketIoEngine::is_loopback_enabled(
    std::uint32_t port_id
) const
{
    return std::find(
        loopback_ports_.begin(),
        loopback_ports_.end(),
        port_id
    ) != loopback_ports_.end();
}

Packet PacketIoEngine::materialize_legacy_packet(
    const RxPacket& received,
    std::uint32_t egress_port
) const
{
    Packet packet;

    const PacketBuffer& buffer = *received.buffer;

    packet.data.assign(
        buffer.data(),
        buffer.data() + buffer.size()
    );

    packet.packet_id = received.metadata.packet_id;
    packet.ingress_port = received.metadata.ingress_port;
    packet.egress_port = egress_port;

    return packet;
}

bool PacketIoEngine::inject_packet(
    std::uint32_t ingress_port,
    const Packet& packet
)
{
    EthernetPort* port = ports_.get_port(ingress_port);

    if (port == nullptr ||
        port->admin_state != PortAdminState::UP ||
        port->oper_state != PortOperState::UP ||
        packet.empty() ||
        packet.size() > port->mtu + 18U) {

        if (port != nullptr) {
            ++port->stats.rx_dropped;
        }

        return false;
    }

    const std::size_t capacity = buffer_pool_.buffer_capacity();

    if (capacity < RX_BUFFER_HEADROOM ||
        packet.size() > capacity - RX_BUFFER_HEADROOM) {
        ++port->stats.rx_dropped;
        return false;
    }

    // Software posts a descriptor for the simulated device.
    if (!rx_ring_.post()) {
        ++port->stats.rx_dropped;
        return false;
    }

    PacketMetadata metadata;
    metadata.packet_id = next_packet_id_++;
    metadata.ingress_port = ingress_port;

    const std::span<const std::uint8_t> frame(
        packet.data.data(),
        packet.size()
    );

    // Simulated device writes the incoming frame.
    if (!rx_ring_.simulate_dma_receive(frame, metadata)) {
        ++port->stats.rx_dropped;
        return false;
    }

    return true;
}

bool PacketIoEngine::transmit_packet(const Packet& packet)
{
    EthernetPort* egress = ports_.get_port(packet.egress_port);

    if (egress == nullptr ||
        egress->admin_state != PortAdminState::UP ||
        egress->oper_state != PortOperState::UP) {

        if (egress != nullptr) {
            ++egress->stats.tx_dropped;
        }

        return false;
    }

    // Acquire a buffer that remains owned by the TX descriptor
    // until software reclaims its completion.
    auto lease = buffer_pool_.acquire_lease();

    if (!lease) {
        ++egress->stats.tx_dropped;
        return false;
    }

    if (!lease->append(packet.data.data(), packet.size())) {
        ++egress->stats.tx_dropped;
        return false;
    }

    PacketMetadata metadata;
    metadata.packet_id = packet.packet_id;
    metadata.ingress_port = packet.ingress_port;
    metadata.egress_port = packet.egress_port;

    // Ownership transfers to the TX ring on successful submission.
    if (!tx_ring_.submit(std::move(lease), metadata)) {
        ++egress->stats.tx_dropped;
        return false;
    }

    // No device service or completion polling happens here.
    return true;
}

std::size_t PacketIoEngine::service_tx(std::size_t budget)
{
    std::size_t serviced = 0;

    while (serviced < budget) {
        if (!tx_ring_.simulate_dma_transmit()) {
            break;
        }

        ++serviced;
    }

    return serviced;
}

std::size_t PacketIoEngine::reclaim_tx(std::size_t budget)
{
    std::size_t reclaimed = 0;

    while (reclaimed < budget) {
        auto completion = tx_ring_.poll();

        if (!completion) {
            break;
        }

        EthernetPort* egress =
            ports_.get_port(completion->metadata.egress_port);

        auto frame = tx_ring_.take_transmitted_frame();

        if (completion->status != TxCompletionStatus::SUCCESS ||
            !frame || egress == nullptr) {

            if (egress != nullptr) {
                ++egress->stats.tx_dropped;
            }

            ++reclaimed;
            continue;
        }

        Packet captured;
        captured.data = std::move(*frame);
        captured.packet_id = completion->metadata.packet_id;
        captured.ingress_port = completion->metadata.ingress_port;
        captured.egress_port = completion->metadata.egress_port;

        egress_queue_.push_back({
            captured.egress_port,
            std::move(captured)
        });

        ++egress->stats.tx_packets;
        egress->stats.tx_bytes += completion->buffer->size();

        // The completion's buffer lease is released when this
        // completion object is destroyed.
        ++reclaimed;
    }

    return reclaimed;
}

std::size_t PacketIoEngine::process_packets(
    std::size_t budget
)
{
    std::size_t processed = 0;

    while (processed < budget &&
           rx_ring_.completed_count() != 0) {

        auto received = rx_ring_.poll();

        if (!received) {
            break;
        }

        EthernetPort* ingress =
            ports_.get_port(received->metadata.ingress_port);

        if (ingress == nullptr) {
            ++processed;
            continue;
        }

        if (received->completion_status != RxCompletionStatus::SUCCESS) {
            ++ingress->stats.rx_dropped;
            ++processed;
            continue;
        }

        if (!is_loopback_enabled(received->metadata.ingress_port)) {
            ++ingress->stats.rx_dropped;
            ++processed;
            continue;
        }

        Packet output = materialize_legacy_packet(
            *received,
            received->metadata.ingress_port
        );

        ++ingress->stats.rx_packets;
        ingress->stats.rx_bytes += output.size();

        // Release the RX buffer before acquiring a TX buffer.
        received->buffer.reset();

        transmit_packet(output);

        ++processed;
    }

    return processed;
}

std::size_t PacketIoEngine::process_packets(
    std::size_t budget,
    DatapathPipeline& pipeline
)
{
    std::size_t processed = 0;

    while (processed < budget &&
           rx_ring_.completed_count() != 0) {

        auto received = rx_ring_.poll();

        if (!received) {
            break;
        }

        EthernetPort* ingress =
            ports_.get_port(received->metadata.ingress_port);

        if (ingress == nullptr) {
            ++processed;
            continue;
        }

        if (received->completion_status != RxCompletionStatus::SUCCESS) {
            ++ingress->stats.rx_dropped;
            ++processed;
            continue;
        }

        if (ingress->admin_state != PortAdminState::UP ||
            ingress->oper_state != PortOperState::UP) {
            ++ingress->stats.rx_dropped;
            ++processed;
            continue;
        }

        Packet input = materialize_legacy_packet(*received);

        ++ingress->stats.rx_packets;
        ingress->stats.rx_bytes += input.size();

        // The datapath now owns its independent Packet representation.
        received->buffer.reset();

        const DatapathResult result = pipeline.process(input);

        if (result.status != DatapathStatus::FORWARDED ||
            result.egress_packets.empty()) {
            ++ingress->stats.rx_dropped;
            ++processed;
            continue;
        }

        // Every egress copy gets its own TX buffer and descriptor.
        for (const Packet& output : result.egress_packets) {
            transmit_packet(output);
        }

        ++processed;
    }

    return processed;
}

std::optional<Packet> PacketIoEngine::capture_packet(
    std::uint32_t egress_port
)
{
    const auto it = std::find_if(
        egress_queue_.begin(),
        egress_queue_.end(),
        [egress_port](const EgressEntry& entry) {
            return entry.port_id == egress_port;
        }
    );

    if (it == egress_queue_.end()) {
        return std::nullopt;
    }

    Packet packet = std::move(it->packet);
    egress_queue_.erase(it);

    return packet;
}

std::size_t PacketIoEngine::pending_ingress() const noexcept
{
    return rx_ring_.completed_count();
}

std::size_t PacketIoEngine::pending_egress() const noexcept
{
    return egress_queue_.size();
}

std::size_t PacketIoEngine::buffers_available() const noexcept
{
    return buffer_pool_.available_count();
}

std::size_t PacketIoEngine::buffers_in_use() const noexcept
{
    return buffer_pool_.in_use_count();
}

std::size_t PacketIoEngine::rx_posted_descriptors() const noexcept
{
    return rx_ring_.posted_count();
}

std::size_t PacketIoEngine::rx_completed_descriptors() const noexcept
{
    return rx_ring_.completed_count();
}

std::size_t PacketIoEngine::tx_posted_descriptors() const noexcept
{
    return tx_ring_.posted_count();
}

std::size_t PacketIoEngine::tx_completed_descriptors() const noexcept
{
    return tx_ring_.completed_count();
}

void PacketIoEngine::reset() noexcept
{
    rx_ring_.reset();
    tx_ring_.reset();

    egress_queue_.clear();
    loopback_ports_.clear();

    next_packet_id_ = 1;
}
