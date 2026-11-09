#pragma once
#include <vector>
#include <string>

class Cluster {
public:
    std::vector<std::string> nodes;

    int getNode(const std::string &key);
};