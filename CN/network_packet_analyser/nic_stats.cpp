#include "nic_stats.h"

#include <iostream>
#include <cstdio>
#include <sstream>

NICStats::NICStats(const std::string& iface)
{
    interface = iface;
}

void NICStats::update()
{
    counters.clear();

    std::string cmd = "ethtool -S " + interface;

    FILE* fp = popen(cmd.c_str(), "r");
    if (!fp)
        return;

    char buffer[512];

    while (fgets(buffer, sizeof(buffer), fp))
    {
        std::string line(buffer);

        size_t colon = line.find(":");
        if (colon == std::string::npos)
            continue;

        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);

        long long val = atoll(value.c_str());

        counters[key] = val;
    }

    pclose(fp);
}

void NICStats::print()
{
    std::cout << "\n===== NIC Hardware Counters =====\n";

    for (auto& [key, value] : counters)
    {
        if (key.find("error") != std::string::npos ||
            key.find("drop") != std::string::npos)
        {
            std::cout << key << " : " << value << std::endl;
        }
    }
}