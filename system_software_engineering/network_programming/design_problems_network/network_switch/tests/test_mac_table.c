#include <assert.h>
#include <stdio.h>

#include "mac_table.h"

static const uint8_t MAC_A[6] = {
    0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA
};

static const uint8_t MAC_B[6] = {
    0xBB, 0xBB, 0xBB, 0xBB, 0xBB, 0xBB
};

static void test_learn_and_lookup(void)
{
    mac_table_t table;
    uint16_t port = 0;

    mac_table_init(&table);

    assert(mac_table_learn(&table, MAC_A, 100, 1, 1000) == 0);

    assert(mac_table_lookup(&table, MAC_A, 100, &port));
    assert(port == 1);
    assert(table.count == 1);

    printf("PASS: MAC learning and lookup\n");
}

static void test_mac_move(void)
{
    mac_table_t table;
    uint16_t port = 0;

    mac_table_init(&table);

    assert(mac_table_learn(&table, MAC_A, 100, 1, 1000) == 0);

    /* Same MAC appears on a different port. */
    assert(mac_table_learn(&table, MAC_A, 100, 2, 2000) == 0);

    assert(mac_table_lookup(&table, MAC_A, 100, &port));
    assert(port == 2);

    assert(table.count == 1);

    printf("PASS: MAC move detection\n");
}

static void test_vlan_isolation(void)
{
    mac_table_t table;
    uint16_t port = 0;

    mac_table_init(&table);

    assert(mac_table_learn(&table, MAC_A, 100, 1, 1000) == 0);
    assert(mac_table_learn(&table, MAC_A, 200, 2, 1000) == 0);

    assert(mac_table_lookup(&table, MAC_A, 100, &port));
    assert(port == 1);

    assert(mac_table_lookup(&table, MAC_A, 200, &port));
    assert(port == 2);

    assert(table.count == 2);

    printf("PASS: VLAN isolation\n");
}

static void test_aging(void)
{
    mac_table_t table;
    uint16_t port = 0;

    mac_table_init(&table);

    assert(mac_table_learn(&table, MAC_A, 100, 1, 1000) == 0);
    assert(mac_table_learn(&table, MAC_B, 100, 2, 5000) == 0);

    size_t removed = mac_table_age(&table, 9999, 5000);

    assert(removed == 1);
    assert(table.count == 1);

    assert(!mac_table_lookup(&table, MAC_A, 100, &port));
    assert(mac_table_lookup(&table, MAC_B, 100, &port));

    assert(port == 2);

    printf("PASS: MAC aging\n");
}

static void test_deleted_entry_reuse(void)
{
    mac_table_t table;
    uint16_t port = 0;

    mac_table_init(&table);

    assert(mac_table_learn(&table, MAC_A, 100, 1, 1000) == 0);

    assert(mac_table_age(&table, 6000, 5000) == 1);

    assert(mac_table_learn(&table, MAC_B, 100, 2, 7000) == 0);

    assert(mac_table_lookup(&table, MAC_B, 100, &port));
    assert(port == 2);

    assert(table.count == 1);

    printf("PASS: Deleted entry reuse\n");
}

static void test_invalid_input(void)
{
    mac_table_t table;

    mac_table_init(&table);

    assert(mac_table_learn(NULL, MAC_A, 100, 1, 1000) == -1);

    assert(mac_table_learn(&table, NULL, 100, 1, 1000) == -1);

    assert(mac_table_learn(&table, MAC_A, 4095, 1, 1000) == -1);

    printf("PASS: Invalid input handling\n");
}

int main(void)
{
    test_learn_and_lookup();
    test_mac_move();
    test_vlan_isolation();
    test_aging();
    test_deleted_entry_reuse();
    test_invalid_input();

    printf("\nAll MAC table tests passed!\n");

    return 0;
}
