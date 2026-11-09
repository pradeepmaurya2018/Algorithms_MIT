#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "ethernet.h"

static void test_untagged_ethernet(void)
{
    packet_t packet = {0};
    ethernet_header_t header = {0};

    uint8_t frame[] = {
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x08, 0x00,
        0x45, 0x00, 0x00, 0x14
    };

    memcpy(packet.data, frame, sizeof(frame));
    packet.len = sizeof(frame);

    assert(ethernet_parse(&packet, &header) == 0);
    assert(header.ether_type == ETH_P_IP);
    assert(header.is_vlan == false);
    assert(header.l3_offset == 14);
    assert(header.dst_mac[0] == 0xAA);
    assert(header.src_mac[0] == 0x11);

    printf("PASS: Untagged Ethernet frame\n");
}

static void test_vlan_ethernet(void)
{
    packet_t packet = {0};
    ethernet_header_t header = {0};

    uint8_t frame[] = {
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
        0x81, 0x00,
        0x00, 0x64,
        0x08, 0x00,
        0x45, 0x00, 0x00, 0x14
    };

    memcpy(packet.data, frame, sizeof(frame));
    packet.len = sizeof(frame);

    assert(ethernet_parse(&packet, &header) == 0);
    assert(header.is_vlan == true);
    assert(header.vlan_id == 100);
    assert(header.ether_type == ETH_P_IP);
    assert(header.l3_offset == 18);

    printf("PASS: VLAN-tagged Ethernet frame\n");
}

static void test_truncated_frame(void)
{
    packet_t packet = {0};
    ethernet_header_t header = {0};

    packet.len = 10;

    assert(ethernet_parse(&packet, &header) == -1);

    printf("PASS: Truncated Ethernet frame\n");
}

int main(void)
{
    test_untagged_ethernet();
    test_vlan_ethernet();
    test_truncated_frame();

    printf("\nAll Ethernet tests passed!\n");

    return 0;
}
