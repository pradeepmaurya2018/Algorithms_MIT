#include <stdio.h>
#include <string.h>

#include "ethernet.h"

static uint16_t read_be16(const uint8_t *data)
{
    return ((uint16_t)data[0] << 8) |
           (uint16_t)data[1];
}

int ethernet_parse(
    const packet_t *packet,
    ethernet_header_t *header)
{
    if (packet == NULL || header == NULL)
        return -1;

    if (packet->len > MAX_PACKET_SIZE)
        return -1;

    if (packet->len < ETH_HEADER_LEN)
        return -1;

    memset(header, 0, sizeof(*header));

    const uint8_t *data = packet->data;

    memcpy(header->dst_mac, data, ETH_ADDR_LEN);
    memcpy(header->src_mac, data + ETH_ADDR_LEN, ETH_ADDR_LEN);

    uint16_t ether_type = read_be16(data + 12);
    size_t offset = ETH_HEADER_LEN;

    if (ether_type == ETH_P_8021Q) {

        if (packet->len < VLAN_HEADER_LEN)
            return -1;

        uint16_t tci = read_be16(data + 14);

        header->is_vlan = true;
        header->vlan_id = tci & 0x0FFF;

        ether_type = read_be16(data + 16);
        offset = VLAN_HEADER_LEN;
    }

    header->ether_type = ether_type;
    header->l3_offset = offset;

    return 0;
}

void ethernet_print(const ethernet_header_t *header)
{
    if (header == NULL)
        return;

    printf("Destination MAC: "
           "%02x:%02x:%02x:%02x:%02x:%02x\n",
           header->dst_mac[0], header->dst_mac[1],
           header->dst_mac[2], header->dst_mac[3],
           header->dst_mac[4], header->dst_mac[5]);

    printf("Source MAC: "
           "%02x:%02x:%02x:%02x:%02x:%02x\n",
           header->src_mac[0], header->src_mac[1],
           header->src_mac[2], header->src_mac[3],
           header->src_mac[4], header->src_mac[5]);

    printf("EtherType: 0x%04x\n", header->ether_type);
    printf("VLAN enabled: %s\n",
           header->is_vlan ? "yes" : "no");

    if (header->is_vlan)
        printf("VLAN ID: %u\n", header->vlan_id);

    printf("L3 offset: %zu\n", header->l3_offset);
}
