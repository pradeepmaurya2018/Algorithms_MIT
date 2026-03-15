#pragma once
#include <map>
#include <string>

class ConsistentHash {
public:
    std::map<size_t,std::string> ring;

    void addNode(const std::string&);
    std::string getNode(const std::string&);
};