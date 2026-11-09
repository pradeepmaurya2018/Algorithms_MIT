#include <stdbool.h>
#include <string.h>

#include "arp.h"
#include "ethernet.h"

static uint16_t read_be16(const uint8_t *data)
{
    return ((uint16_t)data[0] << 8) | data[1];
}

static uint32_t read_be32(const uint8_t *data)
{
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8) |
           (uint32_t)data[3];
}

static void write_be16(uint8_t *data, uint16_t value)
{
    data[0] = (uint8_t)(value >> 8);
    data[1] = (uint8_t)value;
}

static void write_be32(uint8_t *data, uint32_t value)
{
    data[0] = (uint8_t)(value >> 24);
    data[1] = (uint8_t)(value >> 16);
    data[2] = (uint8_t)(value >> 8);
    data[3] = (uint8_t)value;
}

static bool mac_is_unicast(const uint8_t mac[MAC_ADDR_LEN])
{
    if (mac == NULL || (mac[0] & 1U) != 0)
        return false;

    for (size_t i = 0; i < MAC_ADDR_LEN; i++) {
        if (mac[i] != 0)
            return true;
    }

    return false;
}

static void arp_write_header(
    packet_t *packet,
    const uint8_t ethernet_dst[MAC_ADDR_LEN],
    const uint8_t ethernet_src[MAC_ADDR_LEN],
    uint16_t opcode,
    const uint8_t sender_mac[MAC_ADDR_LEN],
    uint32_t sender_ip,
    const uint8_t target_mac[MAC_ADDR_LEN],
    uint32_t target_ip,
    uint16_t egress_port)
{
    memset(packet, 0, sizeof(*packet));

    packet->len = ARP_ETHERNET_FRAME_LEN;
    packet->egress_port = egress_port;

    memcpy(packet->data, ethernet_dst, MAC_ADDR_LEN);
    memcpy(packet->data + 6, ethernet_src, MAC_ADDR_LEN);

    write_be16(packet->data + 12, ETH_P_ARP);

    uint8_t *data = packet->data + ETH_HEADER_LEN;

    write_be16(data, ARP_HARDWARE_ETHERNET);
    write_be16(data + 2, ARP_PROTOCOL_IPV4);

    data[4] = MAC_ADDR_LEN;
    data[5] = 4;

    write_be16(data + 6, opcode);

    memcpy(data + 8, sender_mac, MAC_ADDR_LEN);
    write_be32(data + 14, sender_ip);

    memcpy(data + 18, target_mac, MAC_ADDR_LEN);
    write_be32(data + 24, target_ip);
}

int arp_parse(
    const packet_t *packet,
    size_t offset,
    arp_packet_t *arp)
{
    if (packet == NULL || arp == NULL)
        return -1;

    if (packet->len > MAX_PACKET_SIZE || offset > packet->len)
        return -1;

    if (packet->len - offset < ARP_IPV4_PACKET_LEN)
        return -1;

    const uint8_t *data = packet->data + offset;

    memset(arp, 0, sizeof(*arp));

    arp->hardware_type = read_be16(data);
    arp->protocol_type = read_be16(data + 2);
    arp->hardware_length = data[4];
    arp->protocol_length = data[5];
    arp->opcode = read_be16(data + 6);

    if (arp->hardware_type != ARP_HARDWARE_ETHERNET ||
        arp->protocol_type != ARP_PROTOCOL_IPV4 ||
        arp->hardware_length != MAC_ADDR_LEN ||
        arp->protocol_length != 4)
        return -1;

    if (arp->opcode != ARP_OPCODE_REQUEST &&
        arp->opcode != ARP_OPCODE_REPLY)
        return -1;

    memcpy(arp->sender_mac, data + 8, MAC_ADDR_LEN);
    arp->sender_ip = read_be32(data + 14);

    memcpy(arp->target_mac, data + 18, MAC_ADDR_LEN);
    arp->target_ip = read_be32(data + 24);

    if (!mac_is_unicast(arp->sender_mac))
        return -1;

    return 0;
}

int arp_build_request(
    packet_t *packet,
    const uint8_t sender_mac[MAC_ADDR_LEN],
    uint32_t sender_ip,
    uint32_t target_ip,
    uint16_t egress_port)
{
    static const uint8_t broadcast_mac[MAC_ADDR_LEN] =
        {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    static const uint8_t zero_mac[MAC_ADDR_LEN] =
        {0, 0, 0, 0, 0, 0};

    if (packet == NULL || !mac_is_unicast(sender_mac) ||
        sender_ip == 0 || target_ip == 0 || egress_port == 0)
        return -1;

    arp_write_header(
        packet,
        broadcast_mac,
        sender_mac,
        ARP_OPCODE_REQUEST,
        sender_mac,
        sender_ip,
        zero_mac,
        target_ip,
        egress_port);

    return 0;
}

int arp_build_reply(
    packet_t *packet,
    const uint8_t sender_mac[MAC_ADDR_LEN],
    uint32_t sender_ip,
    const uint8_t target_mac[MAC_ADDR_LEN],
    uint32_t target_ip,
    uint16_t egress_port)
{
    if (packet == NULL ||
        !mac_is_unicast(sender_mac) ||
        !mac_is_unicast(target_mac) ||
        sender_ip == 0 || target_ip == 0 ||
        egress_port == 0)
        return -1;

    arp_write_header(
        packet,
        target_mac,
        sender_mac,
        ARP_OPCODE_REPLY,
        sender_mac,
        sender_ip,
        target_mac,
        target_ip,
        egress_port);

    return 0;
}
