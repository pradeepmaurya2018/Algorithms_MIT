#include <stdio.h>
#include <string.h>

#include "ipv4.h"

#define CHECK(condition, message) do { \
    if (!(condition)) { \
        printf("FAIL: %s\n", message); \
        return 1; \
    } \
    printf("PASS: %s\n", message); \
} while (0)

static uint16_t calculate_test_checksum(const uint8_t *data)
{
    uint32_t sum = 0;

    for (size_t i = 0; i < 20; i += 2) {
        uint16_t word =
            ((uint16_t)data[i] << 8) | data[i + 1];
        sum += word;
    }

    while (sum >> 16)
        sum = (sum & 0xFFFFU) + (sum >> 16);

    return (uint16_t)(~sum & 0xFFFFU);
}

static void create_ipv4_packet(packet_t *packet, uint8_t ttl)
{
    memset(packet, 0, sizeof(*packet));

    packet->len = 34;

    uint8_t *ip = packet->data + 14;

    ip[0] = 0x45;       /* IPv4, IHL = 5 */
    ip[2] = 0x00;
    ip[3] = 0x14;       /* Total length = 20 */
    ip[8] = ttl;
    ip[9] = 17;         /* UDP */

    ip[12] = 10;
    ip[13] = 0;
    ip[14] = 0;
    ip[15] = 1;

    ip[16] = 10;
    ip[17] = 0;
    ip[18] = 0;
    ip[19] = 2;

    uint16_t checksum = calculate_test_checksum(ip);

    ip[10] = (uint8_t)(checksum >> 8);
    ip[11] = (uint8_t)checksum;
}

int main(void)
{
    packet_t packet;
    ipv4_header_t header;

    create_ipv4_packet(&packet, 64);

    CHECK(ipv4_parse(&packet, 14, &header) == 0,
          "Parse IPv4 packet");

    CHECK(ipv4_checksum_valid(&packet, 14, &header),
          "Valid IPv4 checksum");

    packet.data[14 + 12] ^= 0x01;

    CHECK(!ipv4_checksum_valid(&packet, 14, &header),
          "Reject corrupted IPv4 header");

    create_ipv4_packet(&packet, 64);

    CHECK(ipv4_parse(&packet, 14, &header) == 0,
          "Parse packet before TTL update");

    CHECK(ipv4_decrement_ttl(&packet, 14, &header) == 0,
          "Decrement TTL");

    CHECK(header.ttl == 63 && packet.data[14 + 8] == 63,
          "TTL changed from 64 to 63");

    CHECK(ipv4_checksum_valid(&packet, 14, &header),
          "Checksum valid after TTL update");

    create_ipv4_packet(&packet, 1);

    CHECK(ipv4_parse(&packet, 14, &header) == 0,
          "Parse packet with TTL one");

    CHECK(ipv4_decrement_ttl(&packet, 14, &header) == -1,
          "Reject forwarding when TTL is one");

    CHECK(packet.data[14 + 8] == 1,
          "TTL remains unchanged after rejected forwarding");

    printf("\nAll IPv4 checksum tests passed!\n");
    return 0;
}
