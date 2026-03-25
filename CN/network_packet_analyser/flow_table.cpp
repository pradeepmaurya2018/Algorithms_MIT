#include "flow_table.h"
#include <iostream>

FlowTable& FlowTable::get() {
    static FlowTable instance;
    return instance;
}

void FlowTable::update(const FlowKey& key, int bytes) {

    auto& flow = table[key];
    flow.packets++;
    flow.bytes += bytes;
}

void FlowTable::print_top() {

    std::cout << "Active flows: " << table.size() << std::endl;

    int count = 0;

    for (auto& [key, stats] : table) {

        std::cout << "Packets: " << stats.packets
                  << " Bytes: " << stats.bytes
                  << std::endl;

        if (++count > 10)
            break;
    }
}