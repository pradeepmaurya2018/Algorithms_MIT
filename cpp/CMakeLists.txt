cmake_minimum_required(VERSION 3.20)
project(cpp LANGUAGES CXX C)

# ---- Language ----
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 17)

# ---- Default build type ----
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# ---- Unity build (BIG speed boost) ----
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 32)

# ---- ccache ----
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

# ---- Common warnings ----
set(COMMON_WARNINGS
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
)

# ---- Debug flags ----
set(DEBUG_FLAGS
        -O0
        -g3
        -ggdb
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
)

# ---- Sanitizers ----
set(SANITIZERS
        -fsanitize=address,undefined,leak
)

# ---- Release flags ----
set(RELEASE_FLAGS
        -O3
        -march=native
        -DNDEBUG
)

# ---- Function to apply settings ----
function(configure_target target_name)

    # Warnings (all builds)
    target_compile_options(${target_name} PRIVATE ${COMMON_WARNINGS})

    # Debug build
    target_compile_options(${target_name} PRIVATE
            $<$<CONFIG:Debug>:${DEBUG_FLAGS} ${SANITIZERS}
            -D_GLIBCXX_DEBUG
            -D_GLIBCXX_DEBUG_PEDANTIC>
    )

    target_link_options(${target_name} PRIVATE
            $<$<CONFIG:Debug>:${SANITIZERS}>
    )

    # Release build
    target_compile_options(${target_name} PRIVATE
            $<$<CONFIG:Release>:${RELEASE_FLAGS}>
    )

    # Fast linker
    target_link_options(${target_name} PRIVATE -fuse-ld=lld)

    # Precompiled headers
    target_precompile_headers(${target_name} PRIVATE header.h)

endfunction()

# ---- Targets ----
add_executable(bit1 bit_manipulation/bit.cpp)
add_executable(bit2 bit_manipulation/bit2.cpp)
add_executable(bit3 bit_manipulation/bit3.cpp)
add_executable(graph dsa_cpp/graph.cpp)
add_executable(bit4 bit_manipulation/bit4.c)

# ---- Apply config ----
foreach(target bit1 bit2 bit3 graph bit4)
    configure_target(${target})
endforeach()