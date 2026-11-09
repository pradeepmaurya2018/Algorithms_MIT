#include <string.h>

#include "icmp.h"

static uint16_t read_be16(const uint8_t *data)
{
    return ((uint16_t)data[0] << 8) | data[1];
}

static void write_be16(uint8_t *data, uint16_t value)
{
    data[0] = (uint8_t)(value >> 8);
    data[1] = (uint8_t)value;
}

static uint16_t calculate_checksum(
    const uint8_t *data,
    size_t length)
{
    uint32_t sum = 0;

    while (length >= 2) {
        sum += ((uint16_t)data[0] << 8) | data[1];
        data += 2;
        length -= 2;
    }

    if (length != 0)
        sum += (uint16_t)data[0] << 8;

    while ((sum >> 16) != 0)
        sum = (sum & 0xFFFFU) + (sum >> 16);

    return (uint16_t)(~sum & 0xFFFFU);
}

int icmp_parse(
    const packet_t *packet,
    const ipv4_header_t *ip,
    icmp_header_t *icmp)
{
    if (packet == NULL || ip == NULL || icmp == NULL)
        return -1;

    if (packet->len > MAX_PACKET_SIZE)
        return -1;

    if (ip->protocol != ICMP_PROTOCOL)
        return -1;

    if (ip->payload_offset > packet->len ||
        ip->payload_length < ICMP_HEADER_LEN ||
        ip->payload_length > packet->len - ip->payload_offset)
        return -1;

    const uint8_t *data = packet->data + ip->payload_offset;

    memset(icmp, 0, sizeof(*icmp));

    icmp->type = data[0];
    icmp->code = data[1];
    icmp->checksum = read_be16(data + 2);
    icmp->identifier = read_be16(data + 4);
    icmp->sequence = read_be16(data + 6);

    icmp->payload_offset = ip->payload_offset;
    icmp->payload_length = ip->payload_length;

    return 0;
}

bool icmp_checksum_valid(
    const packet_t *packet,
    const icmp_header_t *icmp)
{
    if (packet == NULL || icmp == NULL)
        return false;

    if (packet->len > MAX_PACKET_SIZE ||
        icmp->payload_offset > packet->len ||
        icmp->payload_length < ICMP_HEADER_LEN ||
        icmp->payload_length >
            packet->len - icmp->payload_offset)
        return false;

    return calculate_checksum(
        packet->data + icmp->payload_offset,
        icmp->payload_length) == 0;
}

int icmp_build_echo_reply(
    packet_t *reply,
    const packet_t *request,
    size_t ip_offset,
    const ipv4_header_t *ip)
{
    if (reply == NULL || request == NULL || ip == NULL)
        return -1;

    if (ip->protocol != ICMP_PROTOCOL ||
        request->len > MAX_PACKET_SIZE)
        return -1;

    /*
     * Do not process fragmented packets in this initial
     * local ICMP implementation.
     */
    if ((ip->flags_fragment & 0x3FFFU) != 0)
        return -1;

    if (ip_offset > request->len ||
        ip->header_length < IPV4_MIN_HEADER_LEN ||
        ip->header_length > IPV4_MAX_HEADER_LEN ||
        ip->header_length > request->len - ip_offset ||
        ip->total_length > request->len - ip_offset)
        return -1;

    icmp_header_t icmp;

    if (icmp_parse(request, ip, &icmp) != 0)
        return -1;

    if (!icmp_checksum_valid(request, &icmp))
        return -1;

    if (icmp.type != ICMP_TYPE_ECHO_REQUEST || icmp.code != 0)
        return -1;

    /*
     * Preserve the original Ethernet/VLAN structure,
     * IPv4 options, and ICMP payload.
     */
    *reply = *request;

    uint8_t *ip_data = reply->data + ip_offset;
    uint8_t *icmp_data = reply->data + ip->payload_offset;

    /* Swap IPv4 source and destination addresses. */
    uint8_t address[4];

    memcpy(address, ip_data + 12, sizeof(address));
    memcpy(ip_data + 12, ip_data + 16, sizeof(address));
    memcpy(ip_data + 16, address, sizeof(address));

    /* Set the locally generated response TTL. */
    ip_data[8] = 64;

    /* Change Echo Request to Echo Reply. */
    icmp_data[0] = ICMP_TYPE_ECHO_REPLY;
    icmp_data[1] = 0;

    /* Recalculate ICMP checksum. */
    icmp_data[2] = 0;
    icmp_data[3] = 0;

    uint16_t icmp_checksum =
        calculate_checksum(icmp_data, ip->payload_length);

    write_be16(icmp_data + 2, icmp_checksum);

    /* Recalculate IPv4 header checksum. */
    ip_data[10] = 0;
    ip_data[11] = 0;

    uint16_t ip_checksum =
        calculate_checksum(ip_data, ip->header_length);

    write_be16(ip_data + 10, ip_checksum);

    reply->egress_port = request->ingress_port;

    return 0;
}
