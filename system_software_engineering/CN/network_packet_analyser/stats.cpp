#include "stats.h"
#include <iostream>

Stats& Stats::get() {
    static Stats instance;
    return instance;
}

void Stats::update(int bytes_count) {
    packets++;
    bytes += bytes_count;
}

void Stats::print() {

    std::cout << "Packets: " << packets
              << " Bytes: " << bytes
              << std::endl;
}