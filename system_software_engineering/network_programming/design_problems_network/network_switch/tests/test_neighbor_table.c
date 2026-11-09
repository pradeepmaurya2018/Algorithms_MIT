#include <stdio.h>
#include <string.h>

#include "neighbor_table.h"

#define CHECK(condition, message) do { \
    if (!(condition)) { \
        printf("FAIL: %s\n", message); \
        return 1; \
    } \
    printf("PASS: %s\n", message); \
} while (0)

int main(void)
{
    neighbor_table_t table;
    uint8_t mac[MAC_ADDR_LEN];
    uint16_t port_id = 0;

    const uint8_t host_a[MAC_ADDR_LEN] =
        {0x02, 0x00, 0x00, 0x00, 0x00, 0x0A};

    const uint8_t host_a_new[MAC_ADDR_LEN] =
        {0x02, 0x00, 0x00, 0x00, 0x00, 0x0B};

    const uint8_t multicast_mac[MAC_ADDR_LEN] =
        {0x01, 0x00, 0x5E, 0x00, 0x00, 0x01};

    neighbor_table_init(&table);

    CHECK(table.count == 0, "Initialize neighbor table");

    CHECK(neighbor_table_add(
              &table, 0x0A00000A, 10, 1, host_a) == 0,
          "Add neighbor");

    CHECK(table.count == 1, "Neighbor count incremented");

    CHECK(neighbor_table_lookup(
              &table, 0x0A00000A, 10, mac, &port_id),
          "Lookup neighbor");

    CHECK(port_id == 1, "Lookup returns egress port");

    CHECK(memcmp(mac, host_a, MAC_ADDR_LEN) == 0,
          "Lookup returns correct MAC");

    CHECK(!neighbor_table_lookup(
              &table, 0x0A00000A, 20, mac, &port_id),
          "VLAN isolation");

    CHECK(neighbor_table_add(
              &table, 0x0A00000A, 10, 2, host_a_new) == 0,
          "Update existing neighbor");

    CHECK(table.count == 1, "Update does not increase count");

    CHECK(neighbor_table_lookup(
              &table, 0x0A00000A, 10, mac, &port_id),
          "Lookup updated neighbor");

    CHECK(port_id == 2, "Updated egress port is returned");

    CHECK(memcmp(mac, host_a_new, MAC_ADDR_LEN) == 0,
          "Updated MAC is returned");

    CHECK(neighbor_table_add(
              &table, 0, 10, 1, host_a) == -1,
          "Reject zero IP address");

    CHECK(neighbor_table_add(
              &table, 0x0A00000B, 10, 1, multicast_mac) == -1,
          "Reject multicast MAC");

    CHECK(neighbor_table_add(
              &table, 0x0A00000B, 10, 0, host_a) == -1,
          "Reject zero egress port");

    CHECK(neighbor_table_remove(
              &table, 0x0A00000A, 10),
          "Remove neighbor");

    CHECK(table.count == 0, "Neighbor count decremented");

    CHECK(!neighbor_table_lookup(
              &table, 0x0A00000A, 10, mac, &port_id),
          "Removed neighbor is not found");

    CHECK(!neighbor_table_remove(
              &table, 0x0A00000A, 10),
          "Reject removal of missing neighbor");

    printf("\nAll neighbor table tests passed!\n");
    return 0;
}
