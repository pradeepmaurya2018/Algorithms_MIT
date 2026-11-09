#include "packet_parser.h"
#include "flow_table.h"

#include <iostream>

#include <net/ethernet.h>   // struct ethhdr + ETH_P_IP
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>

void PacketParser::parse(unsigned char* buffer, int size) {

    struct ethhdr* eth = (struct ethhdr*)buffer;

    if (ntohs(eth->h_proto) != ETH_P_IP)
        return;

    struct iphdr* ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));

    FlowKey key{};
    key.src_ip = ip->saddr;
    key.dst_ip = ip->daddr;
    key.protocol = ip->protocol;

    int ip_header_len = ip->ihl * 4;

    if (ip->protocol == IPPROTO_TCP) {

        struct tcphdr* tcp = (struct tcphdr*)
            (buffer + sizeof(struct ethhdr) + ip_header_len);

        key.src_port = ntohs(tcp->source);
        key.dst_port = ntohs(tcp->dest);
    }

    else if (ip->protocol == IPPROTO_UDP) {

        struct udphdr* udp = (struct udphdr*)
            (buffer + sizeof(struct ethhdr) + ip_header_len);

        key.src_port = ntohs(udp->source);
        key.dst_port = ntohs(udp->dest);
    }

    FlowTable::get().update(key, size);
}