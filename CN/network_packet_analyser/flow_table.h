#pragma once
#include <unordered_map>
#include <cstdint>
#include <string>

struct FlowKey {

    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
    uint8_t protocol;

    bool operator==(const FlowKey& other) const {
        return src_ip == other.src_ip &&
               dst_ip == other.dst_ip &&
               src_port == other.src_port &&
               dst_port == other.dst_port &&
               protocol == other.protocol;
    }
};

struct FlowStats {

    uint64_t packets = 0;
    uint64_t bytes = 0;
};

struct FlowHash {

    std::size_t operator()(const FlowKey& k) const {

        return std::hash<uint32_t>()(k.src_ip) ^
               std::hash<uint32_t>()(k.dst_ip) ^
               std::hash<uint16_t>()(k.src_port) ^
               std::hash<uint16_t>()(k.dst_port) ^
               std::hash<uint8_t>()(k.protocol);
    }
};

class FlowTable {

public:
    static FlowTable& get();

    void update(const FlowKey& key, int bytes);
    void print_top();

private:
    std::unordered_map<FlowKey, FlowStats, FlowHash> table;
};