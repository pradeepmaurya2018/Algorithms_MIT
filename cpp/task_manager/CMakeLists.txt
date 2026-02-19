cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
project(task_manager)


add_executable(System_Programming main.cpp
        src/main.cpp
        src/Logger.cpp
        src/Scheduler.cpp
        "src/Task Manager.cpp"
        include/Logger.hpp
        include/Scheduler.hpp
        include/Task.hpp
        "include/Task copy 3.hpp"
        src/Task.cpp)