#include <stdio.h>
#include <string.h>

#include "ipv4.h"

static uint16_t read_be16(const uint8_t *data)
{
    return ((uint16_t)data[0] << 8) |
           (uint16_t)data[1];
}

static uint32_t read_be32(const uint8_t *data)
{
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8) |
           (uint32_t)data[3];
}

int ipv4_parse(
    const packet_t *packet,
    size_t offset,
    ipv4_header_t *header)
{
    if (packet == NULL || header == NULL)
        return -1;

    if (packet->len > MAX_PACKET_SIZE)
        return -1;

    if (offset > packet->len)
        return -1;

    if (packet->len - offset < IPV4_MIN_HEADER_LEN)
        return -1;

    const uint8_t *data = packet->data + offset;

    memset(header, 0, sizeof(*header));

    uint8_t version_ihl = data[0];

    header->version = version_ihl >> 4;
    header->ihl = version_ihl & 0x0F;

    if (header->version != 4)
        return -1;

    if (header->ihl < 5 || header->ihl > 15)
        return -1;

    header->header_length = (size_t)header->ihl * 4;

    if (packet->len - offset < header->header_length)
        return -1;

    header->total_length = read_be16(data + 2);

    if (header->total_length < header->header_length)
        return -1;

    if (header->total_length > packet->len - offset)
        return -1;

    header->identification = read_be16(data + 4);
    header->flags_fragment = read_be16(data + 6);

    header->ttl = data[8];
    header->protocol = data[9];

    header->header_checksum = read_be16(data + 10);

    header->src_ip = read_be32(data + 12);
    header->dst_ip = read_be32(data + 16);

    header->payload_offset = offset + header->header_length;

    header->payload_length =
        header->total_length - header->header_length;

    return 0;
}

void ipv4_print(const ipv4_header_t *header)
{
    if (header == NULL)
        return;

    printf("IPv4 version: %u\n", header->version);
    printf("Header length: %zu\n", header->header_length);
    printf("Total length: %u\n", header->total_length);

    printf("Source IP: %u.%u.%u.%u\n",
           (unsigned)((header->src_ip >> 24) & 0xFF),
           (unsigned)((header->src_ip >> 16) & 0xFF),
           (unsigned)((header->src_ip >> 8) & 0xFF),
           (unsigned)(header->src_ip & 0xFF));

    printf("Destination IP: %u.%u.%u.%u\n",
           (unsigned)((header->dst_ip >> 24) & 0xFF),
           (unsigned)((header->dst_ip >> 16) & 0xFF),
           (unsigned)((header->dst_ip >> 8) & 0xFF),
           (unsigned)(header->dst_ip & 0xFF));

    printf("TTL: %u\n", header->ttl);
    printf("Protocol: %u\n", header->protocol);
}

/*
 * Calculate the Internet checksum over an IPv4 header.
 * The checksum field must be zero before calculation.
 */
static uint16_t ipv4_calculate_checksum(
    const uint8_t *header,
    size_t header_length)
{
    uint32_t sum = 0;

    for (size_t i = 0; i < header_length; i += 2) {
        uint16_t word =
            ((uint16_t)header[i] << 8) |
            (uint16_t)header[i + 1];

        sum += word;
    }

    while ((sum >> 16) != 0)
        sum = (sum & 0xFFFFU) + (sum >> 16);

    return (uint16_t)(~sum & 0xFFFFU);
}

bool ipv4_checksum_valid(
    const packet_t *packet,
    size_t offset,
    const ipv4_header_t *header)
{
    if (packet == NULL || header == NULL)
        return false;

    if (packet->len > MAX_PACKET_SIZE || offset > packet->len)
        return false;

    if (header->header_length < IPV4_MIN_HEADER_LEN ||
        header->header_length > IPV4_MAX_HEADER_LEN ||
        (header->header_length % 2) != 0)
        return false;

    if (header->header_length > packet->len - offset)
        return false;

    const uint8_t *data = packet->data + offset;
    uint32_t sum = 0;

    for (size_t i = 0; i < header->header_length; i += 2) {
        uint16_t word =
            ((uint16_t)data[i] << 8) |
            (uint16_t)data[i + 1];

        sum += word;
    }

    while ((sum >> 16) != 0)
        sum = (sum & 0xFFFFU) + (sum >> 16);

    return sum == 0xFFFFU;
}

int ipv4_decrement_ttl(
    packet_t *packet,
    size_t offset,
    ipv4_header_t *header)
{
    if (packet == NULL || header == NULL)
        return -1;

    if (!ipv4_checksum_valid(packet, offset, header))
        return -1;

    /*
     * A forwarded packet must not leave this router
     * with TTL zero.
     */
    if (header->ttl <= 1)
        return -1;

    uint8_t *data = packet->data + offset;

    data[8]--;

    /* Clear the old checksum. */
    data[10] = 0;
    data[11] = 0;

    uint16_t checksum =
        ipv4_calculate_checksum(data, header->header_length);

    data[10] = (uint8_t)(checksum >> 8);
    data[11] = (uint8_t)(checksum & 0xFF);

    header->ttl--;
    header->header_checksum = checksum;

    return 0;
}
