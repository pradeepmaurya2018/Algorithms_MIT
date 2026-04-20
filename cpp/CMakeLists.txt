cmake_minimum_required(VERSION 3.20)
project(cpp LANGUAGES CXX)

# ---- Language ----
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Build type ----
set(CMAKE_BUILD_TYPE Debug)

# ---- Unity build (BIG speed boost) ----
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 32)

# ---- ccache (safe detection) ----
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

# ---- Targets ----
add_executable(bit1 bit_manipulation/bit.cpp)
add_executable(bit2 bit_manipulation/bit2.cpp)
add_executable(bit3 bit_manipulation/bit3.cpp)
add_executable(graph dsa_cpp/graph.cpp)

# ---- Apply optimizations (CORRECT ORDER) ----
foreach(target bit1 bit2 bit3 graph)

    # ⚡ Fast compile
    target_compile_options(${target} PRIVATE
            -O0
            -g0
            -pipe
            -fno-exceptions
            -fno-rtti
            -fno-unwind-tables
    )

    # ⚡ Fast linker
    target_link_options(${target} PRIVATE -fuse-ld=lld)

    # ⚡ Precompiled headers
    target_precompile_headers(${target} PRIVATE header.h)

endforeach()