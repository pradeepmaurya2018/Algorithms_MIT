#pragma once

#include "hardware/l2_forwarding/l2_forwarding.hpp"
#include "hardware/packet_io/packet.hpp"
#include "hardware/packet_parser/packet_parser.hpp"
#include "hardware/vlan/vlan_classifier.hpp"
#include "hardware/vlan/vlan_manager.hpp"
#include "ipu/port_manager.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

enum class DatapathStatus {
    FORWARDED,
    PARTIALLY_FORWARDED,
    DROPPED,
    PARSE_ERROR,
    VLAN_ERROR
};

enum class PipelineStage {
    NONE,
    PARSER,
    VLAN_CLASSIFICATION,
    L2_FORWARDING,
    EGRESS_PREPARATION
};

enum class PipelineDropReason {
    NONE,
    INVALID_ETHERNET_HEADER,
    INVALID_IPV4_HEADER,
    INVALID_IPV6_HEADER,
    VLAN_REJECTED,
    FORWARDING_REJECTED,
    DESTINATION_ON_INGRESS_PORT,
    NO_EGRESS_PORT,
    EGRESS_PREPARATION_FAILED
};

struct PipelineStats {
    std::uint64_t packets_received{0};

    std::uint64_t parser_success{0};
    std::uint64_t parser_dropped{0};

    std::uint64_t vlan_classified{0};
    std::uint64_t vlan_dropped{0};

    std::uint64_t forwarding_success{0};
    std::uint64_t forwarding_dropped{0};

    std::uint64_t egress_prepared{0};
    std::uint64_t egress_preparation_failed{0};

    std::uint64_t packets_forwarded{0};
    std::uint64_t packets_partially_forwarded{0};
    std::uint64_t packets_dropped{0};
};

struct DatapathResult {
    DatapathStatus status{DatapathStatus::DROPPED};

    PipelineStage failed_stage{PipelineStage::NONE};
    PipelineDropReason drop_reason{PipelineDropReason::NONE};

    PacketParseStatus parse_status{
        PacketParseStatus::ETHERNET_ERROR
    };

    VlanClassifyStatus vlan_status{
        VlanClassifyStatus::INVALID_PORT
    };

    std::uint16_t classified_vlan{0};

    L2ForwardingDecision forwarding;

    std::size_t egress_prepared_count{0};
    std::size_t egress_failed_count{0};

    std::vector<Packet> egress_packets;
};

class DatapathPipeline {
public:
    explicit DatapathPipeline(PortManager& ports);

    DatapathResult process(const Packet& packet);

    const PipelineStats& stats() const noexcept;
    void reset_stats() noexcept;

    VlanManager& vlans() noexcept;
    const VlanManager& vlans() const noexcept;

    L2ForwardingEngine& forwarding() noexcept;
    const L2ForwardingEngine& forwarding() const noexcept;

private:
    bool prepare_egress_packet(
        Packet& packet,
        std::uint16_t vlan_id) const;

    VlanManager vlans_;
    VlanClassifier classifier_;
    L2ForwardingEngine forwarding_;

    PipelineStats stats_{};
};
