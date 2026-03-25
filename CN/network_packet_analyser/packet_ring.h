#pragma once

#include <string>
#include <linux/if_packet.h>

class PacketRing
{
public:
    PacketRing(const std::string& iface, int queue_id);

    bool init();
    void run();

private:
    std::string interface;
    int queue;
    int sockfd;

    void* ring;

    struct tpacket_req req;
};