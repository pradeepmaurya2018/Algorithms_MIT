cmake_minimum_required(VERSION 3.20)
project(cpp LANGUAGES CXX)

# ---- Language ----
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- ccache (instant rebuilds) ----
set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
set(CMAKE_C_COMPILER_LAUNCHER ccache)

# ---- Fast compile flags (dev) ----
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g0 -pipe -fno-exceptions -fno-rtti -fno-unwind-tables")

# ---- Fast linker ----
set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=lld")
set(CMAKE_BUILD_TYPE Debug)
# ---- Targets (ONE main per executable) ----
add_executable(bit1 bit_manipulation/bit.cpp)
#add_executable(bit2 bit_manipulation/bit2.cpp)
#add_executable(bit3 bit_manipulation/bit3.cpp)
#add_executable(graph dsa_cpp/graph.cpp)

# ---- PCH (attach AFTER target creation) ----

target_precompile_headers(bit1 PRIVATE header.h)
#target_precompile_headers(bit2 PRIVATE header.h)
#target_precompile_headers(bit3 PRIVATE header.h)
#target_precompile_headers(graph PRIVATE header.h)
