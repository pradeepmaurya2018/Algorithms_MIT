#pragma once
#include <vector>
#include <string>

class Replication {
public:
    std::vector<int> replicas;

    void addReplica(int fd);
    void replicate(const std::string &cmd);
};