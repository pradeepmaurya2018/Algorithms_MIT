#include "packet_ring.h"
#include "stats.h"
#include "flow_table.h"
#include "nic_stats.h"

#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

int main(int argc, char* argv[])
{
    std::string interface = "eth0";

    if (argc > 1)
        interface = argv[1];

    int threads = std::thread::hardware_concurrency();

    std::cout << "NIC Packet Analyzer\n";
    std::cout << "Interface: " << interface << "\n";
    std::cout << "Worker threads: " << threads << "\n";

    NICStats nic_stats(interface);

    std::vector<std::thread> workers;

    for (int i = 0; i < threads; i++)
    {
        workers.emplace_back([&, i]()
        {
            PacketRing ring(interface, i);

            if (!ring.init())
            {
                std::cerr << "Queue init failed\n";
                return;
            }

            ring.run();
        });
    }

    while (true)
    {
        std::this_thread::sleep_for(
            std::chrono::seconds(2));

        std::cout << "\n===== Statistics =====\n";

        Stats::get().print();

        std::cout << "\n===== Top Flows =====\n";

        FlowTable::get().print_top();

        nic_stats.update();
        nic_stats.print();
    }

    for (auto& w : workers)
        w.join();

    return 0;
}