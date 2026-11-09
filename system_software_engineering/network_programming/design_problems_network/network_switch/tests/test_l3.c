#include <stdio.h>
#include <string.h>

#include "switch.h"
#include "ipv4.h"

#define CHECK(condition, message) do { \
    if (!(condition)) { \
        printf("FAIL: %s\n", message); \
        switch_destroy(&sw); \
        return 1; \
    } \
    printf("PASS: %s\n", message); \
} while (0)

static uint16_t checksum(const uint8_t *data, size_t length)
{
    uint32_t sum = 0;

    for (size_t i = 0; i < length; i += 2) {
        uint16_t word =
            ((uint16_t)data[i] << 8) | data[i + 1];
        sum += word;
    }

    while (sum >> 16)
        sum = (sum & 0xFFFFU) + (sum >> 16);

    return (uint16_t)(~sum & 0xFFFFU);
}

static void create_packet(
    packet_t *packet,
    const uint8_t router_mac[MAC_ADDR_LEN],
    const uint8_t host_mac[MAC_ADDR_LEN],
    uint32_t src_ip,
    uint32_t dst_ip,
    uint8_t ttl)
{
    memset(packet, 0, sizeof(*packet));

    packet->len = 14 + 20;
    packet->ingress_port = 1;

    memcpy(packet->data, router_mac, MAC_ADDR_LEN);
    memcpy(packet->data + 6, host_mac, MAC_ADDR_LEN);

    packet->data[12] = 0x08;
    packet->data[13] = 0x00;

    uint8_t *ip = packet->data + 14;

    ip[0] = 0x45;
    ip[2] = 0x00;
    ip[3] = 0x14;
    ip[8] = ttl;
    ip[9] = 17;

    ip[12] = (uint8_t)(src_ip >> 24);
    ip[13] = (uint8_t)(src_ip >> 16);
    ip[14] = (uint8_t)(src_ip >> 8);
    ip[15] = (uint8_t)src_ip;

    ip[16] = (uint8_t)(dst_ip >> 24);
    ip[17] = (uint8_t)(dst_ip >> 16);
    ip[18] = (uint8_t)(dst_ip >> 8);
    ip[19] = (uint8_t)dst_ip;

    uint16_t sum = checksum(ip, 20);

    ip[10] = (uint8_t)(sum >> 8);
    ip[11] = (uint8_t)sum;
}

int main(void)
{
    switch_t sw;
    switch_result_t result;
    packet_t input;

    const uint8_t router_vlan10[MAC_ADDR_LEN] =
        {0x02, 0x00, 0x00, 0x00, 0x10, 0x01};

    const uint8_t router_vlan20[MAC_ADDR_LEN] =
        {0x02, 0x00, 0x00, 0x00, 0x20, 0x01};

    const uint8_t host_a[MAC_ADDR_LEN] =
        {0x02, 0xAA, 0x00, 0x00, 0x00, 0x01};

    const uint8_t host_b[MAC_ADDR_LEN] =
        {0x02, 0xBB, 0x00, 0x00, 0x00, 0x02};

    const uint32_t host_a_ip = 0x0A000002; /* 10.0.0.2 */
    const uint32_t host_b_ip = 0x0A001402; /* 10.0.20.2 */

    switch_init(&sw);

    CHECK(switch_add_access_port(&sw, 1, 10) == 0,
          "Create VLAN 10 access port");

    CHECK(switch_add_access_port(&sw, 2, 20) == 0,
          "Create VLAN 20 access port");

    CHECK(switch_add_router_interface(
              &sw, 10, 0x0A000001, router_vlan10) == 0,
          "Configure VLAN 10 router interface");

    CHECK(switch_add_router_interface(
              &sw, 20, 0x0A001401, router_vlan20) == 0,
          "Configure VLAN 20 router interface");

    CHECK(switch_add_route(
              &sw, 0x0A001400, 24, 0, 0, 20) == 0,
          "Install VLAN 20 connected route");

    CHECK(switch_add_neighbor(
              &sw, host_b_ip, 20, 2, host_b) == 0,
          "Add VLAN 20 destination neighbor");

    create_packet(
        &input,
        router_vlan10,
        host_a,
        host_a_ip,
        host_b_ip,
        64);

    CHECK(switch_process_packet(
              &sw, &input, 100, &result) == 0,
          "Process inter-VLAN packet");

    CHECK(result.action == SWITCH_ACTION_ROUTED,
          "Select Layer 3 routing path");

    CHECK(result.egress_count == 1,
          "Generate one routed egress packet");

    CHECK(result.egress_ports[0] == 2,
          "Forward through VLAN 20 port");

    packet_t *output = &result.egress_packets[0];

    CHECK(memcmp(output->data, host_b, MAC_ADDR_LEN) == 0,
          "Rewrite destination MAC to neighbor MAC");

    CHECK(memcmp(
              output->data + 6,
              router_vlan20,
              MAC_ADDR_LEN) == 0,
          "Rewrite source MAC to egress router MAC");

    CHECK(output->data[14 + 8] == 63,
          "Decrement IPv4 TTL");

    ethernet_header_t eth;
    ipv4_header_t ip;

    CHECK(ethernet_parse(output, &eth) == 0,
          "Parse routed Ethernet frame");

    CHECK(ipv4_parse(output, eth.l3_offset, &ip) == 0,
          "Parse routed IPv4 packet");

    CHECK(ipv4_checksum_valid(output, eth.l3_offset, &ip),
          "Validate checksum after routing");

    CHECK(output->len == 34,
          "Preserve untagged access-port frame length");

    /* TTL expiration must prevent forwarding. */
    create_packet(
        &input,
        router_vlan10,
        host_a,
        host_a_ip,
        host_b_ip,
        1);

    CHECK(switch_process_packet(
              &sw, &input, 200, &result) == 0,
          "Process packet with TTL one");

    CHECK(result.action == SWITCH_ACTION_DROP &&
          result.egress_count == 0,
          "Drop packet whose TTL would expire");

    switch_destroy(&sw);

    printf("\nAll Layer 3 integration tests passed!\n");
    return 0;
}
