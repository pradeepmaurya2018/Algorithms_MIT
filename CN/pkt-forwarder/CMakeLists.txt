cmake_minimum_required(VERSION 3.16)
project(pkt-forwarder C)

set(CMAKE_C_STANDARD 11)
add_compile_definitions(_GNU_SOURCE)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DPDK REQUIRED libdpdk)

include_directories(${DPDK_INCLUDE_DIRS})
link_directories(${DPDK_LIBRARY_DIRS})
add_compile_options(${DPDK_CFLAGS_OTHER})

add_executable(pkt-forwarder
    main.c
    port.c
    ring.c
    classifier.c
    qos.c
    stats.c
)

target_link_libraries(pkt-forwarder
    ${DPDK_LIBRARIES}
    pthread dl numa)
