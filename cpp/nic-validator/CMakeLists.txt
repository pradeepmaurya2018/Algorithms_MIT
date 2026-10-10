cmake_minimum_required(VERSION 3.16)
project(nic-validator VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)

include_directories(include)

add_executable(nic-validator
    src/main.cpp
    src/core/logger.cpp
    src/core/test_runner.cpp
    src/nic/linux_nic.cpp
    src/tests/pci_discovery_test.cpp
)

target_link_libraries(nic-validator PRIVATE pthread)
