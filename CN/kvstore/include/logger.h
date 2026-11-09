#pragma once
#include <iostream>
#include <chrono>

inline void logInfo(const std::string &msg){

    auto now = std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now());

    std::cout << "[INFO] "
              << std::ctime(&now)
              << " " << msg << std::endl;
}