#include "core/test_runner.hpp"
#include "tests/ipu_init_test.hpp"
#include "tests/ipu_port_init_test.hpp"
#include "tests/ipu_packet_io_test.hpp"
#include "tests/ipu_ethernet_parser_test.hpp"
#include "tests/ipu_ipv4_parser_test.hpp"
#include "tests/ipu_packet_parser_test.hpp"
#include "tests/ipu_ipv6_parser_test.hpp"
#include "tests/ipu_l2_forwarding_test.hpp"
#include "tests/ipu_datapath_pipeline_test.hpp"
#include "tests/ipu_packet_io_datapath_test.hpp"
#include "tests/ipu_packet_buffer_io_test.hpp"
#include "tests/ipu_rx_descriptor_ring_test.hpp"
#include "tests/ipu_tx_descriptor_ring_test.hpp"
#include "tests/ipu_vlan_test.hpp"

#include <iostream>
#include <memory>
#include <string>

static void usage(const char* argv0)
{
    std::cout
        << "Usage:\n"
        << "  " << argv0 << " --test IPU_INIT\n"
        << "  " << argv0 << " --test IPU_PORT_INIT\n"
        << "  " << argv0 << " --test IPU_PACKET_IO\n"
        << "  " << argv0 << " --test IPU_ETH_PARSER\n"
        << "  " << argv0 << " --test IPU_IPV4_PARSER\n"
        << "  " << argv0 << " --test IPU_PACKET_PARSER\n"
        << "  " << argv0 << " --all\n"
        << "  " << argv0 << " --help\n"
        << "\n"
        << "Available tests:\n"
        << "  IPU_INIT\n"
        << "  IPU_PORT_INIT\n"
        << "  IPU_PACKET_IO\n"
        << "  IPU_ETH_PARSER\n"
        << "  IPU_IPV4_PARSER\n"
        << "  IPU_PACKET_PARSER\n";
}

int main(int argc, char* argv[])
{
    std::string selected_test;
    bool run_all = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--test" && i + 1 < argc) {
            selected_test = argv[++i];
        } else if (arg == "--all") {
            run_all = true;
        } else if (arg == "--help" || arg == "-h") {
            usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Unknown argument: " << arg << '\n';
            usage(argv[0]);
            return 2;
        }
    }

    if (!run_all && selected_test.empty()) {
        usage(argv[0]);
        return 2;
    }

    TestRunner runner;

    if (run_all || selected_test == "IPU_INIT") {
        runner.add(std::make_unique<IpuInitTest>());
    }

    if (run_all || selected_test == "IPU_PORT_INIT") {
        runner.add(std::make_unique<IpuPortInitTest>());
    }

    if (run_all || selected_test == "IPU_PACKET_IO") {
        runner.add(std::make_unique<IpuPacketIoTest>());
    }

    if (run_all || selected_test == "IPU_ETH_PARSER") {
        runner.add(std::make_unique<IpuEthernetParserTest>());
    }

    if (run_all || selected_test == "IPU_IPV4_PARSER") {
        runner.add(std::make_unique<IpuIpv4ParserTest>());
    }

    if (run_all || selected_test == "IPU_PACKET_PARSER") {
        runner.add(std::make_unique<IpuPacketParserTest>());
    }

    if (run_all || selected_test == "IPU_IPV6_PARSER") {
        runner.add(std::make_unique<IpuIpv6ParserTest>());
    }

    if (run_all || selected_test == "IPU_L2_FORWARDING") {
        runner.add(std::make_unique<IpuL2ForwardingTest>());
    }

    if (run_all || selected_test == "IPU_DATAPATH_PIPELINE") {
        runner.add(std::make_unique<IpuDatapathPipelineTest>());
    }

    if (run_all || selected_test == "IPU_DEVICE_DATAPATH") {
        runner.add(std::make_unique<IpuPacketIoDatapathTest>());
    }

    if (run_all || selected_test == "IPU_PACKET_BUFFER_IO") {
        runner.add(std::make_unique<IpuPacketBufferIoTest>());
    }

    if (run_all || selected_test == "IPU_RX_DESCRIPTOR_RING") {
        runner.add(std::make_unique<IpuRxDescriptorRingTest>());
    }

    if (run_all || selected_test == "IPU_TX_DESCRIPTOR_RING") {
        runner.add(std::make_unique<IpuTxDescriptorRingTest>());
    }

    if (run_all || selected_test == "IPU_VLAN") {
        runner.add(std::make_unique<IpuVlanTest>());
    }

    if (!run_all &&
        selected_test != "IPU_INIT" &&
        selected_test != "IPU_PORT_INIT" &&
        selected_test != "IPU_PACKET_IO" &&
        selected_test != "IPU_ETH_PARSER" &&
        selected_test != "IPU_IPV4_PARSER" &&
        selected_test != "IPU_PACKET_PARSER" &&
        selected_test != "IPU_IPV6_PARSER" &&
        selected_test != "IPU_L2_FORWARDING" &&
        selected_test != "IPU_DATAPATH_PIPELINE" &&
        selected_test != "IPU_DEVICE_DATAPATH" &&
        selected_test != "IPU_PACKET_BUFFER_IO" &&
        selected_test != "IPU_RX_DESCRIPTOR_RING" &&
        selected_test != "IPU_TX_DESCRIPTOR_RING" &&
        selected_test != "IPU_VLAN"){
        std::cerr << "Unknown test: " << selected_test << '\n';
        usage(argv[0]);
        return 2;
    }

    return runner.run_all();
}
