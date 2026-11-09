#include <assert.h>
#include <stdio.h>

#include "route_table.h"

#define IP(a, b, c, d) \
    (((uint32_t)(a) << 24) | \
     ((uint32_t)(b) << 16) | \
     ((uint32_t)(c) << 8)  | \
     (uint32_t)(d))

static void test_basic_lpm(void)
{
    route_table_t table;
    route_entry_t result;

    route_table_init(&table);

    assert(route_table_insert(
        &table, IP(10, 0, 0, 0), 8,
        IP(192, 168, 1, 1), 1, 100) == 0);

    assert(route_table_insert(
        &table, IP(10, 1, 0, 0), 16,
        IP(192, 168, 1, 2), 2, 100) == 0);

    assert(route_table_insert(
        &table, IP(10, 1, 2, 0), 24,
        IP(192, 168, 1, 3), 3, 200) == 0);

    assert(route_table_lookup(
        &table, IP(10, 1, 2, 9), &result));

    assert(result.prefix_length == 24);
    assert(result.next_hop == IP(192, 168, 1, 3));
    assert(result.egress_port == 3);

    printf("PASS: Longest prefix match\n");

    route_table_destroy(&table);
}

static void test_less_specific_match(void)
{
    route_table_t table;
    route_entry_t result;

    route_table_init(&table);

    assert(route_table_insert(
        &table, IP(10, 0, 0, 0), 8,
        IP(192, 168, 1, 1), 1, 100) == 0);

    assert(route_table_insert(
        &table, IP(10, 1, 0, 0), 16,
        IP(192, 168, 1, 2), 2, 100) == 0);

    assert(route_table_lookup(
        &table, IP(10, 2, 1, 5), &result));

    assert(result.prefix_length == 8);
    assert(result.egress_port == 1);

    printf("PASS: Less specific route lookup\n");

    route_table_destroy(&table);
}

static void test_default_route(void)
{
    route_table_t table;
    route_entry_t result;

    route_table_init(&table);

    assert(route_table_insert(
        &table, 0, 0,
        IP(192, 168, 1, 254), 4, 100) == 0);

    assert(route_table_lookup(
        &table, IP(8, 8, 8, 8), &result));

    assert(result.prefix_length == 0);
    assert(result.egress_port == 4);

    printf("PASS: Default route\n");

    route_table_destroy(&table);
}

static void test_no_route(void)
{
    route_table_t table;
    route_entry_t result;

    route_table_init(&table);

    assert(route_table_insert(
        &table, IP(10, 0, 0, 0), 8,
        IP(192, 168, 1, 1), 1, 100) == 0);

    assert(!route_table_lookup(
        &table, IP(172, 16, 0, 1), &result));

    printf("PASS: No route found\n");

    route_table_destroy(&table);
}

static void test_host_route(void)
{
    route_table_t table;
    route_entry_t result;

    route_table_init(&table);

    assert(route_table_insert(
        &table, IP(10, 1, 2, 9), 32,
        IP(192, 168, 1, 10), 5, 300) == 0);

    assert(route_table_lookup(
        &table, IP(10, 1, 2, 9), &result));

    assert(result.prefix_length == 32);
    assert(result.egress_port == 5);

    assert(!route_table_lookup(
        &table, IP(10, 1, 2, 10), &result));

    printf("PASS: Host route /32\n");

    route_table_destroy(&table);
}

static void test_route_replacement(void)
{
    route_table_t table;
    route_entry_t result;

    route_table_init(&table);

    assert(route_table_insert(
        &table, IP(10, 1, 0, 0), 16,
        IP(192, 168, 1, 1), 1, 100) == 0);

    assert(route_table_insert(
        &table, IP(10, 1, 0, 0), 16,
        IP(192, 168, 1, 2), 2, 200) == 0);

    assert(table.route_count == 1);

    assert(route_table_lookup(
        &table, IP(10, 1, 2, 9), &result));

    assert(result.next_hop == IP(192, 168, 1, 2));
    assert(result.egress_port == 2);

    printf("PASS: Route replacement\n");

    route_table_destroy(&table);
}

static void test_prefix_normalization(void)
{
    route_table_t table;
    route_entry_t result;

    route_table_init(&table);

    /* Host bits are normalized for a /24 route. */
    assert(route_table_insert(
        &table, IP(10, 1, 2, 99), 24,
        IP(192, 168, 1, 1), 1, 100) == 0);

    assert(route_table_lookup(
        &table, IP(10, 1, 2, 200), &result));

    assert(result.network == IP(10, 1, 2, 0));

    printf("PASS: Prefix normalization\n");

    route_table_destroy(&table);
}

static void test_invalid_prefix(void)
{
    route_table_t table;

    route_table_init(&table);

    assert(route_table_insert(
        &table, IP(10, 0, 0, 0), 33,
        0, 1, 100) == -1);

    printf("PASS: Invalid prefix rejection\n");

    route_table_destroy(&table);
}

int main(void)
{
    test_basic_lpm();
    test_less_specific_match();
    test_default_route();
    test_no_route();
    test_host_route();
    test_route_replacement();
    test_prefix_normalization();
    test_invalid_prefix();

    printf("\nAll LPM tests passed!\n");

    return 0;
}
