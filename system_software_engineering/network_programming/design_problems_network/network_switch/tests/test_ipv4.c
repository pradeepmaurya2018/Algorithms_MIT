#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ipv4.h"

static void test_ipv4_parse(void)
{
    packet_t packet = {0};
    ipv4_header_t header = {0};

    uint8_t frame[] = {
        /* Ethernet header */
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x08, 0x00,

        /* IPv4 header */
        0x45, 0x00,
        0x00, 0x18,
        0x12, 0x34,
        0x40, 0x00,
        0x40, 0x06,
        0x00, 0x00,
        0xC0, 0xA8, 0x0A, 0x0A,
        0xC0, 0xA8, 0x14, 0x0A,

        /* Payload */
        0xDE, 0xAD, 0xBE, 0xEF
    };

    memcpy(packet.data, frame, sizeof(frame));
    packet.len = sizeof(frame);

    assert(ipv4_parse(&packet, 14, &header) == 0);

    assert(header.version == 4);
    assert(header.ihl == 5);
    assert(header.header_length == 20);
    assert(header.total_length == 24);

    assert(header.src_ip == 0xC0A80A0A);
    assert(header.dst_ip == 0xC0A8140A);

    assert(header.ttl == 64);
    assert(header.protocol == 6);

    assert(header.payload_offset == 34);
    assert(header.payload_length == 4);

    printf("PASS: IPv4 header parsing\n");
}

static void test_invalid_version(void)
{
    packet_t packet = {0};
    ipv4_header_t header;

    packet.len = 34;
    packet.data[14] = 0x65;

    assert(ipv4_parse(&packet, 14, &header) == -1);

    printf("PASS: Invalid IPv4 version\n");
}

static void test_invalid_ihl(void)
{
    packet_t packet = {0};
    ipv4_header_t header;

    packet.len = 34;
    packet.data[14] = 0x44;

    assert(ipv4_parse(&packet, 14, &header) == -1);

    printf("PASS: Invalid IPv4 IHL\n");
}

static void test_invalid_total_length(void)
{
    packet_t packet = {0};
    ipv4_header_t header;

    packet.len = 34;

    packet.data[14] = 0x45;

    /* Total length smaller than IPv4 header. */
    packet.data[16] = 0x00;
    packet.data[17] = 0x10;

    assert(ipv4_parse(&packet, 14, &header) == -1);

    printf("PASS: Invalid IPv4 total length\n");
}

static void test_truncated_packet(void)
{
    packet_t packet = {0};
    ipv4_header_t header;

    packet.len = 20;

    assert(ipv4_parse(&packet, 14, &header) == -1);

    printf("PASS: Truncated IPv4 packet\n");
}

static void test_ipv4_options(void)
{
    packet_t packet = {0};
    ipv4_header_t header;

    packet.len = 38;

    packet.data[14] = 0x46; /* Version 4, IHL 6 */
    packet.data[16] = 0x00;
    packet.data[17] = 0x18;

    assert(ipv4_parse(&packet, 14, &header) == 0);

    assert(header.header_length == 24);
    assert(header.payload_offset == 38);
    assert(header.payload_length == 0);

    printf("PASS: IPv4 options parsing\n");
}

int main(void)
{
    test_ipv4_parse();
    test_invalid_version();
    test_invalid_ihl();
    test_invalid_total_length();
    test_truncated_packet();
    test_ipv4_options();

    printf("\nAll IPv4 tests passed!\n");

    return 0;
}
