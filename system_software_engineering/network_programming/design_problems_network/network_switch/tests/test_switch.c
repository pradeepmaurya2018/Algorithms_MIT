#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "switch.h"

static const uint8_t MAC_A[6] = {
    0x02, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA
};

static const uint8_t MAC_B[6] = {
    0x02, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB
};

static const uint8_t MAC_C[6] = {
    0x02, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC
};

static const uint8_t BROADCAST_MAC[6] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

static const uint8_t MULTICAST_MAC[6] = {
    0x01, 0x00, 0x5E, 0x00, 0x00, 0x01
};

static void make_packet(
    packet_t *packet,
    const uint8_t *src,
    const uint8_t *dst,
    uint16_t ingress_port)
{
    memset(packet, 0, sizeof(*packet));

    memcpy(packet->data, dst, 6);
    memcpy(packet->data + 6, src, 6);

    packet->data[12] = 0x08;
    packet->data[13] = 0x00;

    packet->data[14] = 0x45;

    packet->len = 15;
    packet->ingress_port = ingress_port;
}

static void setup_switch(switch_t *sw)
{
    switch_init(sw);

    assert(switch_add_port(sw, 1, 100) == 0);
    assert(switch_add_port(sw, 2, 100) == 0);
    assert(switch_add_port(sw, 3, 100) == 0);
    assert(switch_add_port(sw, 4, 200) == 0);
}

static void test_unknown_unicast_flood(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_switch(&sw);

    make_packet(&packet, MAC_A, MAC_B, 1);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    assert(result.action == SWITCH_ACTION_FLOOD);
    assert(result.egress_count == 2);

    assert(result.egress_ports[0] == 2);
    assert(result.egress_ports[1] == 3);

    printf("PASS: Unknown unicast flooding\n");
}

static void test_known_unicast(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_switch(&sw);

    /* First learn MAC_B on port 2. */
    make_packet(&packet, MAC_B, MAC_A, 2);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    /* Now MAC_A sends to MAC_B. */
    make_packet(&packet, MAC_A, MAC_B, 1);

    assert(switch_process_packet(&sw, &packet, 2000, &result) == 0);

    assert(result.action == SWITCH_ACTION_UNICAST);
    assert(result.egress_count == 1);
    assert(result.egress_ports[0] == 2);

    printf("PASS: Known unicast forwarding\n");
}

static void test_broadcast_flood(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_switch(&sw);

    make_packet(&packet, MAC_A, BROADCAST_MAC, 1);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    assert(result.action == SWITCH_ACTION_FLOOD);
    assert(result.egress_count == 2);

    printf("PASS: Broadcast flooding\n");
}

static void test_multicast_flood(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_switch(&sw);

    make_packet(&packet, MAC_A, MULTICAST_MAC, 1);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    assert(result.action == SWITCH_ACTION_FLOOD);
    assert(result.egress_count == 2);

    printf("PASS: Multicast flooding\n");
}

static void test_same_port_filtering(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_switch(&sw);

    /* Learn MAC_A on port 1. */
    make_packet(&packet, MAC_A, MAC_B, 1);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    /* Another frame for MAC_A arrives on port 1. */
    make_packet(&packet, MAC_C, MAC_A, 1);

    /* Correct ingress port is 1. */
    packet.ingress_port = 1;

    assert(switch_process_packet(&sw, &packet, 2000, &result) == 0);

    assert(result.action == SWITCH_ACTION_FILTERED);
    assert(result.egress_count == 0);

    printf("PASS: Same-port filtering\n");
}

static void test_vlan_isolation(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_switch(&sw);

    make_packet(&packet, MAC_A, MAC_B, 1);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    /*
     * Port 4 belongs to VLAN 200.
     * It must not receive VLAN 100 flooding.
     */
    assert(result.egress_count == 2);

    for (size_t i = 0; i < result.egress_count; i++) {
        assert(result.egress_ports[i] != 4);
    }

    printf("PASS: VLAN isolation\n");
}

static void test_disabled_egress_port(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_switch(&sw);

    assert(switch_set_port_state(&sw, 3, false) == 0);

    make_packet(&packet, MAC_A, MAC_B, 1);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    assert(result.action == SWITCH_ACTION_FLOOD);
    assert(result.egress_count == 1);
    assert(result.egress_ports[0] == 2);

    printf("PASS: Disabled egress port\n");
}

static void test_disabled_ingress_port(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_switch(&sw);

    assert(switch_set_port_state(&sw, 1, false) == 0);

    make_packet(&packet, MAC_A, MAC_B, 1);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    assert(result.action == SWITCH_ACTION_DROP);
    assert(result.egress_count == 0);

    printf("PASS: Disabled ingress port\n");
}

int main(void)
{
    test_unknown_unicast_flood();
    test_known_unicast();
    test_broadcast_flood();
    test_multicast_flood();
    test_same_port_filtering();
    test_vlan_isolation();
    test_disabled_egress_port();
    test_disabled_ingress_port();

    printf("\nAll switch forwarding tests passed!\n");

    return 0;
}
