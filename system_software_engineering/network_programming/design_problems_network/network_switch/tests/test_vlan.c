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

static void make_frame(
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

static void make_vlan_frame(
    packet_t *packet,
    const uint8_t *src,
    const uint8_t *dst,
    uint16_t vlan_id,
    uint16_t ingress_port)
{
    memset(packet, 0, sizeof(*packet));

    memcpy(packet->data, dst, 6);
    memcpy(packet->data + 6, src, 6);

    packet->data[12] = 0x81;
    packet->data[13] = 0x00;

    packet->data[14] = (uint8_t)(vlan_id >> 8);
    packet->data[15] = (uint8_t)vlan_id;

    packet->data[16] = 0x08;
    packet->data[17] = 0x00;

    packet->data[18] = 0x45;

    packet->len = 19;
    packet->ingress_port = ingress_port;
}

static void setup_vlan_switch(switch_t *sw)
{
    switch_init(sw);

    assert(switch_add_access_port(sw, 1, 10) == 0);
    assert(switch_add_access_port(sw, 2, 10) == 0);

    assert(switch_add_access_port(sw, 3, 20) == 0);

    assert(switch_add_trunk_port(sw, 4) == 0);

    assert(switch_port_allow_vlan(sw, 4, 10) == 0);
    assert(switch_port_allow_vlan(sw, 4, 20) == 0);
}

static void test_access_to_trunk(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_vlan_switch(&sw);

    /*
     * Learn destination MAC_B on trunk port 4.
     */
    make_vlan_frame(&packet, MAC_B, MAC_A, 10, 4);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    /*
     * MAC_A sends an untagged packet from access port 1.
     */
    make_frame(&packet, MAC_A, MAC_B, 1);

    assert(switch_process_packet(&sw, &packet, 2000, &result) == 0);

    assert(result.action == SWITCH_ACTION_UNICAST);
    assert(result.egress_count == 1);
    assert(result.egress_ports[0] == 4);

    packet_t *out = &result.egress_packets[0];

    assert(out->len == packet.len + 4);

    assert(out->data[12] == 0x81);
    assert(out->data[13] == 0x00);

    assert(out->data[14] == 0x00);
    assert(out->data[15] == 10);

    assert(out->data[16] == 0x08);
    assert(out->data[17] == 0x00);

    printf("PASS: Access to trunk VLAN tagging\n");
}

static void test_trunk_to_access(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_vlan_switch(&sw);

    /*
     * Learn MAC_B on access port 2.
     */
    make_frame(&packet, MAC_B, MAC_A, 2);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    /*
     * MAC_A sends a tagged VLAN 10 frame from trunk port 4.
     */
    make_vlan_frame(&packet, MAC_A, MAC_B, 10, 4);

    assert(switch_process_packet(&sw, &packet, 2000, &result) == 0);

    assert(result.action == SWITCH_ACTION_UNICAST);
    assert(result.egress_ports[0] == 2);

    packet_t *out = &result.egress_packets[0];

    assert(out->len == packet.len - 4);

    assert(out->data[12] == 0x08);
    assert(out->data[13] == 0x00);

    assert(out->data[14] == 0x45);

    printf("PASS: Trunk to access VLAN tag removal\n");
}

static void test_trunk_to_trunk(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_vlan_switch(&sw);

    assert(switch_add_trunk_port(&sw, 5) == 0);
    assert(switch_port_allow_vlan(&sw, 5, 10) == 0);

    make_vlan_frame(&packet, MAC_B, MAC_A, 10, 5);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    make_vlan_frame(&packet, MAC_A, MAC_B, 10, 4);

    assert(switch_process_packet(&sw, &packet, 2000, &result) == 0);

    assert(result.action == SWITCH_ACTION_UNICAST);
    assert(result.egress_ports[0] == 5);

    packet_t *out = &result.egress_packets[0];

    assert(out->len == packet.len);
    assert(out->data[12] == 0x81);
    assert(out->data[15] == 10);

    printf("PASS: Trunk to trunk VLAN forwarding\n");
}

static void test_disallowed_vlan(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_vlan_switch(&sw);

    make_vlan_frame(&packet, MAC_A, MAC_B, 30, 4);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    assert(result.action == SWITCH_ACTION_DROP);
    assert(result.egress_count == 0);

    printf("PASS: Disallowed VLAN rejection\n");
}

static void test_untagged_trunk_ingress(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_vlan_switch(&sw);

    make_frame(&packet, MAC_A, MAC_B, 4);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    assert(result.action == SWITCH_ACTION_DROP);

    printf("PASS: Untagged trunk ingress rejection\n");
}

static void test_tagged_access_ingress(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_vlan_switch(&sw);

    make_vlan_frame(&packet, MAC_A, MAC_B, 20, 1);

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    assert(result.action == SWITCH_ACTION_DROP);

    printf("PASS: Tagged access ingress rejection\n");
}

static void test_priority_tagged_access_ingress(void)
{
    switch_t sw;
    packet_t packet;
    switch_result_t result;

    setup_vlan_switch(&sw);

    make_vlan_frame(&packet, MAC_A, MAC_B, 0, 1);

    /* PCP=5, DEI=0, VID=0 */
    packet.data[14] = 0xA0;
    packet.data[15] = 0x00;

    assert(switch_process_packet(&sw, &packet, 1000, &result) == 0);

    assert(result.action == SWITCH_ACTION_FLOOD);
    assert(result.vlan_id == 10);

    printf("PASS: Priority-tagged access ingress classification\n");
}

int main(void)
{
    test_access_to_trunk();
    test_trunk_to_access();
    test_trunk_to_trunk();

    test_disallowed_vlan();
    test_untagged_trunk_ingress();
    test_tagged_access_ingress();
    test_priority_tagged_access_ingress();

    printf("\nAll VLAN tests passed!\n");

    return 0;
}
