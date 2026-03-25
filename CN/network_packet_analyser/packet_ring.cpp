#include "packet_ring.h"
#include "packet_parser.h"
#include "stats.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

PacketRing::PacketRing(const std::string& iface, int q)
{
    interface = iface;
    queue_id = q;
}

bool PacketRing::init()
{
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0)
    {
        perror("socket");
        return false;
    }

    int version = TPACKET_V1;

    if (setsockopt(sockfd,
                   SOL_PACKET,
                   PACKET_VERSION,
                   &version,
                   sizeof(version)) < 0)
    {
        perror("PACKET_VERSION");
        return false;
    }

    req.tp_block_size = 1 << 20;
    req.tp_frame_size = 2048;
    req.tp_block_nr = 64;
    req.tp_frame_nr =
        (req.tp_block_size * req.tp_block_nr) / req.tp_frame_size;

    if (setsockopt(sockfd,
                   SOL_PACKET,
                   PACKET_RX_RING,
                   &req,
                   sizeof(req)) < 0)
    {
        perror("PACKET_RX_RING");
        return false;
    }

    size_t mmap_size = req.tp_block_size * req.tp_block_nr;

    ring = mmap(nullptr,
                mmap_size,
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                sockfd,
                0);

    if (ring == MAP_FAILED)
    {
        perror("mmap");
        return false;
    }

    struct ifreq ifr{};
    strncpy(ifr.ifr_name, interface.c_str(), IFNAMSIZ);

    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1)
    {
        perror("ioctl");
        return false;
    }

    struct sockaddr_ll sll{};
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifr.ifr_ifindex;
    sll.sll_protocol = htons(ETH_P_ALL);

    if (bind(sockfd,
             (struct sockaddr*)&sll,
             sizeof(sll)) < 0)
    {
        perror("bind");
        return false;
    }

    return true;
}

void PacketRing::run()
{
    unsigned int frame = 0;

    while (true)
    {
        struct tpacket_hdr* hdr =
            (struct tpacket_hdr*)((uint8_t*)ring +
                                  (frame * req.tp_frame_size));

        if (!(hdr->tp_status & TP_STATUS_USER))
        {
            continue;
        }

        uint8_t* packet =
            (uint8_t*)hdr + hdr->tp_mac;

        int length = hdr->tp_len;

        Stats::get().update(length);

        PacketParser::parse(packet, length);

        hdr->tp_status = TP_STATUS_KERNEL;

        frame = (frame + 1) % req.tp_frame_nr;
    }
}