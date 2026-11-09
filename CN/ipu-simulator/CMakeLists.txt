cmake_minimum_required(VERSION 3.20)

project(ipu-simulator
    VERSION 0.1.0
    DESCRIPTION "Software IPU Simulator and Validation Framework"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wshadow
)

add_executable(ipu-simulator
    src/main.cpp
    src/core/test_runner.cpp
    src/ipu/ipu_simulator.cpp
    src/ipu/port_manager.cpp
    src/hardware/packet_buffer/packet_buffer.cpp
   src/hardware/packet_buffer/buffer_pool.cpp
   src/tests/ipu_rx_descriptor_ring_test.cpp
   src/hardware/rx_descriptor_ring/rx_descriptor_ring.cpp
   src/tests/ipu_tx_descriptor_ring_test.cpp
   src/hardware/tx_descriptor_ring/tx_descriptor_ring.cpp
   src/hardware/packet_io/packet_io.cpp
    src/hardware/ethernet_parser/ethernet_parser.cpp
    src/hardware/ipv4_parser/ipv4_parser.cpp
    src/hardware/packet_parser/packet_parser.cpp
    src/tests/ipu_init_test.cpp
    src/tests/ipu_port_init_test.cpp
    src/tests/ipu_packet_io_test.cpp
    src/tests/ipu_ethernet_parser_test.cpp
    src/tests/ipu_ipv4_parser_test.cpp
    src/tests/ipu_packet_parser_test.cpp
        src/hardware/ipv6_parser/ipv6_parser.cpp
        src/tests/ipu_ipv6_parser_test.cpp
    src/hardware/l2_forwarding/l2_forwarding.cpp
    src/tests/ipu_l2_forwarding_test.cpp
    src/hardware/datapath_pipeline/datapath_pipeline.cpp
    src/tests/ipu_datapath_pipeline_test.cpp
    src/tests/ipu_packet_io_datapath_test.cpp
   src/tests/ipu_packet_buffer_io_test.cpp
    src/hardware/vlan/vlan_manager.cpp
    src/hardware/vlan/vlan_classifier.cpp
    src/tests/ipu_vlan_test.cpp
)

target_include_directories(ipu-simulator
    PRIVATE include
)
