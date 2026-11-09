#include "hardware/datapath_pipeline/datapath_pipeline.hpp"

#include <cstddef>
#include <cstdint>
#include <utility>

namespace {

constexpr std::size_t ETH_HEADER_SIZE = 14;
constexpr std::size_t VLAN_HEADER_SIZE = 18;
constexpr std::uint16_t VLAN_TPID = 0x8100;

} // namespace

DatapathPipeline::DatapathPipeline(PortManager& ports)
    : vlans_(ports),
      classifier_(vlans_),
      forwarding_(ports, vlans_)
{
}

bool DatapathPipeline::prepare_egress_packet(
    Packet& packet,
    std::uint16_t vlan_id) const
{
    if (packet.data.size() < ETH_HEADER_SIZE) {
        return false;
    }

    const bool tagged =
        packet.data[12] == 0x81 &&
        packet.data[13] == 0x00;

    const bool trunk = vlans_.is_trunk_port(packet.egress_port);

    if (!vlans_.is_member(packet.egress_port, vlan_id)) {
        return false;
    }

    if (trunk) {
        if (tagged) {
            if (packet.data.size() < VLAN_HEADER_SIZE) {
                return false;
            }

            const std::uint16_t existing_vlan =
                static_cast<std::uint16_t>(
                    ((static_cast<std::uint16_t>(packet.data[14]) << 8U) |
                     packet.data[15]) & 0x0FFFU);

            return existing_vlan == vlan_id;
        }

        const std::uint16_t tci = vlan_id & 0x0FFFU;

        packet.data.insert(
            packet.data.begin() + 12,
            {
                0x81,
                0x00,
                static_cast<std::uint8_t>(tci >> 8U),
                static_cast<std::uint8_t>(tci & 0xFFU)
            });

        return true;
    }

    // Access-port egress must be untagged.
    if (tagged) {
        if (packet.data.size() < VLAN_HEADER_SIZE) {
            return false;
        }

        packet.data.erase(
            packet.data.begin() + 12,
            packet.data.begin() + 16);
    }

    return true;
}


DatapathResult DatapathPipeline::process(const Packet& packet)
{
    DatapathResult result;

    ++stats_.packets_received;

    // Stage 1: Ethernet and L3 parsing.
    const PacketParseResult parsed =
        PacketParser::parse(packet.data);

    result.parse_status = parsed.status;

    if (parsed.status != PacketParseStatus::OK) {
        result.status = DatapathStatus::PARSE_ERROR;
        result.failed_stage = PipelineStage::PARSER;

        switch (parsed.status) {
        case PacketParseStatus::ETHERNET_ERROR:
            result.drop_reason =
                PipelineDropReason::INVALID_ETHERNET_HEADER;
            break;

        case PacketParseStatus::IPV4_ERROR:
            result.drop_reason =
                PipelineDropReason::INVALID_IPV4_HEADER;
            break;

        case PacketParseStatus::IPV6_ERROR:
            result.drop_reason =
                PipelineDropReason::INVALID_IPV6_HEADER;
            break;

        case PacketParseStatus::OK:
            break;
        }

        ++stats_.parser_dropped;
        ++stats_.packets_dropped;
        return result;
    }

    ++stats_.parser_success;

    // Stage 2: VLAN classification.
    const VlanClassification classification =
        classifier_.classify(
            parsed.packet.ethernet,
            packet.ingress_port);

    result.vlan_status = classification.status;

    if (!classification.ok()) {
        result.status = DatapathStatus::VLAN_ERROR;
        result.failed_stage = PipelineStage::VLAN_CLASSIFICATION;
        result.drop_reason = PipelineDropReason::VLAN_REJECTED;

        ++stats_.vlan_dropped;
        ++stats_.packets_dropped;
        return result;
    }

    ++stats_.vlan_classified;

    result.classified_vlan = classification.vlan_id;

    // Stage 3: VLAN-aware L2 forwarding.
    result.forwarding = forwarding_.process(
        parsed.packet,
        packet.ingress_port,
        classification.vlan_id);

    if (result.forwarding.action == L2ForwardingAction::DROP ||
        result.forwarding.egress_ports.empty()) {

        result.status = DatapathStatus::DROPPED;
        result.failed_stage = PipelineStage::L2_FORWARDING;

        if (result.forwarding.drop_reason ==
            L2DropReason::DESTINATION_ON_INGRESS_PORT) {

            result.drop_reason =
                PipelineDropReason::DESTINATION_ON_INGRESS_PORT;
        } else if (result.forwarding.egress_ports.empty()) {
            result.drop_reason =
                PipelineDropReason::NO_EGRESS_PORT;
        } else {
            result.drop_reason =
                PipelineDropReason::FORWARDING_REJECTED;
        }

        ++stats_.forwarding_dropped;
        ++stats_.packets_dropped;
        return result;
    }

    ++stats_.forwarding_success;

    // Stage 4: Prepare each egress packet independently.
    for (const std::uint32_t egress_port :
         result.forwarding.egress_ports) {

        Packet output = packet;

        output.ingress_port = packet.ingress_port;
        output.egress_port = egress_port;

        if (!prepare_egress_packet(
                output,
                classification.vlan_id)) {

            ++result.egress_failed_count;
            ++stats_.egress_preparation_failed;
            continue;
        }

        ++result.egress_prepared_count;
        ++stats_.egress_prepared;

        result.egress_packets.push_back(std::move(output));
    }

    if (result.egress_packets.empty()) {
        result.status = DatapathStatus::DROPPED;
        result.failed_stage = PipelineStage::EGRESS_PREPARATION;
        result.drop_reason =
            PipelineDropReason::EGRESS_PREPARATION_FAILED;

        ++stats_.packets_dropped;
        return result;
    }

    if (result.egress_failed_count != 0) {
        result.status = DatapathStatus::PARTIALLY_FORWARDED;
        result.failed_stage = PipelineStage::EGRESS_PREPARATION;
        result.drop_reason =
            PipelineDropReason::EGRESS_PREPARATION_FAILED;

        ++stats_.packets_partially_forwarded;
        return result;
    }

    result.status = DatapathStatus::FORWARDED;

    ++stats_.packets_forwarded;

    return result;
}

const PipelineStats& DatapathPipeline::stats() const noexcept
{
    return stats_;
}

void DatapathPipeline::reset_stats() noexcept
{
    stats_ = PipelineStats{};
}

VlanManager& DatapathPipeline::vlans() noexcept
{
    return vlans_;
}

const VlanManager& DatapathPipeline::vlans() const noexcept
{
    return vlans_;
}

L2ForwardingEngine& DatapathPipeline::forwarding() noexcept
{
    return forwarding_;
}

const L2ForwardingEngine&
DatapathPipeline::forwarding() const noexcept
{
    return forwarding_;
}
