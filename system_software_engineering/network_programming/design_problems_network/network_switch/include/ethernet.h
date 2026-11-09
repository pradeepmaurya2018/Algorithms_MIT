#ifndef ETHERNET_H
#define ETHERNET_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "packet.h"

#define ETH_ADDR_LEN 6
#define ETH_HEADER_LEN 14
#define VLAN_HEADER_LEN 18

#define ETH_P_IP       0x0800
#define ETH_P_ARP      0x0806
#define ETH_P_IPV6     0x86DD
#define ETH_P_8021Q    0x8100

typedef struct {
    uint8_t dst_mac[ETH_ADDR_LEN];
    uint8_t src_mac[ETH_ADDR_LEN];
    uint16_t ether_type;
    bool is_vlan;
    uint16_t vlan_id;
    size_t l3_offset;
} ethernet_header_t;

int ethernet_parse(
    const packet_t *packet,
    ethernet_header_t *header
);

void ethernet_print(const ethernet_header_t *header);

#endif
