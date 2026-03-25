#include "packet_capture.h"
#include "packet_parser.h"
#include "stats.h"

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>

PacketCapture::PacketCapture(const std::string& interface)
{
    iface = interface;
}

bool PacketCapture::start()
{
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (sockfd < 0)
    {
        perror("socket");
        return false;
    }

    struct ifreq ifr{};
    strncpy(ifr.ifr_name, iface.c_str(), IFNAMSIZ);

    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1)
    {
        perror("ioctl");
        return false;
    }

    struct sockaddr_ll sll{};
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ALL);

    if (bind(sockfd, (struct sockaddr*)&sll, sizeof(sll)) == -1)
    {
        perror("bind");
        return false;
    }

    unsigned char buffer[65536];

    while (true)
    {
        ssize_t bytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, nullptr, nullptr);

        if (bytes > 0)
        {
            Stats::get().update(bytes);
            PacketParser::parse(buffer, bytes);
        }
    }

    return true;
}