#include "consistent_hash.h"
#include <functional>

void ConsistentHash::addNode(const std::string &node){
    std::hash<std::string> h;
    ring[h(node)] = node;
}

std::string ConsistentHash::getNode(const std::string &key){

    std::hash<std::string> h;
    size_t pos = h(key);

    auto it = ring.lower_bound(pos);

    if(it == ring.end())
        return ring.begin()->second;

    return it->second;
}