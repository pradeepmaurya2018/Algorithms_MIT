#pragma once

#include <atomic>
#include <cstdint>

class Stats {

public:
    static Stats& get();

    void update(int bytes);
    void print();

private:
    Stats() = default;

    std::atomic<uint64_t> packets{0};
    std::atomic<uint64_t> bytes{0};
};