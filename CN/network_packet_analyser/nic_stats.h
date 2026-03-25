#pragma once
#include <string>
#include <map>

class NICStats
{
public:
    NICStats(const std::string& iface);

    void update();
    void print();

private:
    std::string interface;
    std::map<std::string, long long> counters;
};