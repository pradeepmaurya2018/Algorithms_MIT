#ifndef ARP_H
#define ARP_H

#include <stddef.h>
#include <stdint.h>

#include "packet.h"
#include "mac_table.h"

#define ARP_ETHERTYPE       0x0806
#define ARP_HARDWARE_ETHERNET 1
#define ARP_PROTOCOL_IPV4   0x0800

#define ARP_OPCODE_REQUEST  1
#define ARP_OPCODE_REPLY    2

#define ARP_IPV4_PACKET_LEN 28
#define ARP_ETHERNET_FRAME_LEN 42

typedef struct {
    uint16_t hardware_type;
    uint16_t protocol_type;
    uint8_t hardware_length;
    uint8_t protocol_length;
    uint16_t opcode;

    uint8_t sender_mac[MAC_ADDR_LEN];
    uint32_t sender_ip;

    uint8_t target_mac[MAC_ADDR_LEN];
    uint32_t target_ip;
} arp_packet_t;

int arp_parse(
    const packet_t *packet,
    size_t offset,
    arp_packet_t *arp
);

int arp_build_request(
    packet_t *packet,
    const uint8_t sender_mac[MAC_ADDR_LEN],
    uint32_t sender_ip,
    uint32_t target_ip,
    uint16_t egress_port
);

int arp_build_reply(
    packet_t *packet,
    const uint8_t sender_mac[MAC_ADDR_LEN],
    uint32_t sender_ip,
    const uint8_t target_mac[MAC_ADDR_LEN],
    uint32_t target_ip,
    uint16_t egress_port
);

#endif
