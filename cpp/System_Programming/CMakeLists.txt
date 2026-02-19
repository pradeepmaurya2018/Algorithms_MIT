cmake_minimum_required(VERSION 3.28)

project(System_Programming LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(System_Programming
        src/main.cpp
        src/Logger.cpp
        src/Scheduler.cpp
        src/Task.cpp
)

target_include_directories(System_Programming
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
