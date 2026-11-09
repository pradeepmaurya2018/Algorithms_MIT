#include "tests/ipu_datapath_pipeline_test.hpp"

#include "hardware/datapath_pipeline/datapath_pipeline.hpp"

#include <cstdint>
#include <vector>

namespace {

using Mac = MacAddress;

Mac make_mac(
    std::uint8_t a,
    std::uint8_t b,
    std::uint8_t c,
    std::uint8_t d,
    std::uint8_t e,
    std::uint8_t f)
{
    return {a, b, c, d, e, f};
}

Packet make_ethernet_packet(
    const Mac& source,
    const Mac& destination,
    std::uint32_t ingress_port)
{
    Packet packet;

    // 60-byte Ethernet frame.
    packet.data.resize(60, 0);

    for (std::size_t i = 0; i < 6; ++i) {
        packet.data[i] = destination[i];
        packet.data[6 + i] = source[i];
    }

    // EtherType: ARP (0x0806).
    packet.data[12] = 0x08;
    packet.data[13] = 0x06;

    packet.ingress_port = ingress_port;

    return packet;
}


Packet make_ipv4_packet(
    const Mac& source,
    const Mac& destination,
    std::uint32_t ingress_port)
{
    Packet packet = make_ethernet_packet(
        source, destination, ingress_port);

    packet.data[12] = 0x08;
    packet.data[13] = 0x00;

    // IPv4 header: version 4, IHL 5, total length 20.
    packet.data[14] = 0x45;
    packet.data[15] = 0x00;
    packet.data[16] = 0x00;
    packet.data[17] = 0x14;
    packet.data[18] = 0x00;
    packet.data[19] = 0x01;
    packet.data[20] = 0x00;
    packet.data[21] = 0x00;
    packet.data[22] = 64;
    packet.data[23] = 17;
    packet.data[24] = 0x00;
    packet.data[25] = 0x00;

    // Source: 192.0.2.1, destination: 192.0.2.2.
    packet.data[26] = 192;
    packet.data[27] = 0;
    packet.data[28] = 2;
    packet.data[29] = 1;

    packet.data[30] = 192;
    packet.data[31] = 0;
    packet.data[32] = 2;
    packet.data[33] = 2;

    // Calculate the IPv4 header checksum.
    std::uint32_t sum = 0;

    for (std::size_t i = 14; i < 34; i += 2) {
        const std::uint16_t word =
            static_cast<std::uint16_t>(
                (static_cast<std::uint16_t>(packet.data[i]) << 8U) |
                packet.data[i + 1]);

        sum += word;
    }

    while ((sum >> 16U) != 0) {
        sum = (sum & 0xFFFFU) + (sum >> 16U);
    }

    const std::uint16_t checksum =
        static_cast<std::uint16_t>(~sum);

    packet.data[24] =
        static_cast<std::uint8_t>(checksum >> 8U);

    packet.data[25] =
        static_cast<std::uint8_t>(checksum & 0xFFU);

    return packet;
}

Packet make_invalid_ipv6_packet(
    const Mac& source,
    const Mac& destination,
    std::uint32_t ingress_port)
{
    Packet packet = make_ethernet_packet(
        source, destination, ingress_port);

    packet.data[12] = 0x86;
    packet.data[13] = 0xDD;

    // IPv6 base header starts at byte 14.
    // Version 4 is deliberately invalid for IPv6.
    packet.data[14] = 0x40;

    return packet;
}

} // namespace

std::string IpuDatapathPipelineTest::name() const
{
    return "IPU_DATAPATH_PIPELINE";
}

TestResult IpuDatapathPipelineTest::run()
{
    PortManager ports;

    if (!ports.initialize(4)) {
        return {
            TestStatus::ERROR,
            name(),
            "Port initialization failed"
        };
    }

    for (std::uint32_t id = 0; id < 4; ++id) {
        if (!ports.set_admin_state(id, PortAdminState::UP) ||
            !ports.set_link_present(id, true)) {
            return {
                TestStatus::ERROR,
                name(),
                "Failed to enable ports"
            };
        }
    }

    DatapathPipeline pipeline(ports);

    const Mac source =
        make_mac(0x02, 0, 0, 0, 0, 1);

    const Mac destination =
        make_mac(0x02, 0, 0, 0, 0, 2);

    // Learn the destination on port 1.
    if (!pipeline.forwarding().learn(0, destination, 1)) {
        return {
            TestStatus::FAIL,
            name(),
            "FDB learning failed"
        };
    }

    // Inject packet on port 0.
    const Packet input =
        make_ethernet_packet(source, destination, 0);

    const DatapathResult result = pipeline.process(input);

    if (result.status != DatapathStatus::FORWARDED) {
        return {
            TestStatus::FAIL,
            name(),
            "Datapath did not forward known unicast packet"
        };
    }

    if (result.egress_packets.size() != 1) {
        return {
            TestStatus::FAIL,
            name(),
            "Unexpected number of egress packets"
        };
    }

    const Packet& output = result.egress_packets[0];

    if (output.ingress_port != 0 ||
        output.egress_port != 1 ||
        output.data != input.data) {
        return {
            TestStatus::FAIL,
            name(),
            "Egress packet metadata or contents incorrect"
        };
    }

    // Test unknown unicast flooding.
    const Mac unknown_destination =
        make_mac(0x02, 0, 0, 0, 0, 99);

    const Packet unknown_input =
        make_ethernet_packet(
            source, unknown_destination, 0);

    const DatapathResult flood_result =
        pipeline.process(unknown_input);

    if (flood_result.status != DatapathStatus::FORWARDED ||
        flood_result.egress_packets.size() != 3) {
        return {
            TestStatus::FAIL,
            name(),
            "Unknown-unicast flooding failed"
        };
    }

    for (const Packet& output_packet :
         flood_result.egress_packets) {
        if (output_packet.ingress_port != 0 ||
            output_packet.egress_port == 0) {
            return {
                TestStatus::FAIL,
                name(),
                "Flood output contains incorrect port metadata"
            };
        }
    }

    // Test malformed Ethernet input.
    Packet malformed;
    malformed.ingress_port = 0;
    malformed.data.resize(5, 0);

    const DatapathResult malformed_result =
        pipeline.process(malformed);

    if (malformed_result.status != DatapathStatus::PARSE_ERROR) {
        return {
            TestStatus::FAIL,
            name(),
            "Malformed packet was not rejected"
        };
    }


    // Validate malformed-packet diagnostics.
    if (malformed_result.failed_stage != PipelineStage::PARSER ||
        malformed_result.drop_reason !=
            PipelineDropReason::INVALID_ETHERNET_HEADER) {

        return {
            TestStatus::FAIL,
            name(),
            "Malformed packet diagnostic mismatch"
        };
    }

    // Validate aggregate pipeline statistics.
    const PipelineStats& stats = pipeline.stats();

    if (stats.packets_received != 3 ||
        stats.parser_success != 2 ||
        stats.parser_dropped != 1 ||
        stats.vlan_classified != 2 ||
        stats.vlan_dropped != 0 ||
        stats.forwarding_success != 2 ||
        stats.forwarding_dropped != 0 ||
        stats.egress_prepared != 4 ||
        stats.egress_preparation_failed != 0 ||
        stats.packets_forwarded != 2 ||
        stats.packets_partially_forwarded != 0 ||
        stats.packets_dropped != 1) {

        return {
            TestStatus::FAIL,
            name(),
            "Pipeline statistics mismatch"
        };
    }

    // Validate statistics reset.
    pipeline.reset_stats();

    if (pipeline.stats().packets_received != 0 ||
        pipeline.stats().parser_success != 0 ||
        pipeline.stats().packets_dropped != 0) {

        return {
            TestStatus::FAIL,
            name(),
            "Pipeline statistics reset failed"
        };
    }


    // FI-02: Invalid IPv4 header checksum.
    Packet bad_checksum = make_ipv4_packet(
        source, destination, 0);

    bad_checksum.data[24] ^= 0x01;

    const DatapathResult checksum_result =
        pipeline.process(bad_checksum);

    if (checksum_result.status != DatapathStatus::PARSE_ERROR ||
        checksum_result.failed_stage != PipelineStage::PARSER ||
        checksum_result.drop_reason !=
            PipelineDropReason::INVALID_IPV4_HEADER ||
        checksum_result.parse_status != PacketParseStatus::IPV4_ERROR ||
        !checksum_result.egress_packets.empty()) {

        return {
            TestStatus::FAIL,
            name(),
            "Invalid IPv4 checksum was not rejected correctly"
        };
    }

    // FI-03: Truncated IPv4 header.
    Packet truncated_ipv4 = make_ipv4_packet(
        source, destination, 0);

    truncated_ipv4.data.resize(24);

    const DatapathResult truncated_result =
        pipeline.process(truncated_ipv4);

    if (truncated_result.status != DatapathStatus::PARSE_ERROR ||
        truncated_result.failed_stage != PipelineStage::PARSER ||
        truncated_result.drop_reason !=
            PipelineDropReason::INVALID_IPV4_HEADER ||
        truncated_result.parse_status != PacketParseStatus::IPV4_ERROR ||
        !truncated_result.egress_packets.empty()) {

        return {
            TestStatus::FAIL,
            name(),
            "Truncated IPv4 header was not rejected correctly"
        };
    }

    // FI-04: Invalid IPv6 version.
    Packet invalid_ipv6 = make_invalid_ipv6_packet(
        source, destination, 0);

    const DatapathResult ipv6_result =
        pipeline.process(invalid_ipv6);

    if (ipv6_result.status != DatapathStatus::PARSE_ERROR ||
        ipv6_result.failed_stage != PipelineStage::PARSER ||
        ipv6_result.drop_reason !=
            PipelineDropReason::INVALID_IPV6_HEADER ||
        ipv6_result.parse_status != PacketParseStatus::IPV6_ERROR ||
        !ipv6_result.egress_packets.empty()) {

        return {
            TestStatus::FAIL,
            name(),
            "Invalid IPv6 version was not rejected correctly"
        };
    }

    // Verify parser fault counters.
    const PipelineStats& fault_stats = pipeline.stats();

    if (fault_stats.packets_received != 3 ||
        fault_stats.parser_success != 0 ||
        fault_stats.parser_dropped != 3 ||
        fault_stats.vlan_classified != 0 ||
        fault_stats.forwarding_success != 0 ||
        fault_stats.packets_dropped != 3) {

        return {
            TestStatus::FAIL,
            name(),
            "Parser fault-injection statistics mismatch"
        };
    }

    // Confirm that parser failures never reached forwarding.
    if (fault_stats.egress_prepared != 0 ||
        fault_stats.egress_preparation_failed != 0) {

        return {
            TestStatus::FAIL,
            name(),
            "Parser failure unexpectedly reached egress preparation"
        };
    }

    return {
        TestStatus::PASS,
        name(),
        "Parser-to-FDB-to-egress pipeline, known unicast, "
        "flooding and malformed packet handling validated"
    };
}
