CC = gcc

CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic -g -O0 \
         -Iinclude

SWITCH_SRCS = src/ethernet.c src/mac_table.c src/switch.c \
              src/ipv4.c src/route_table.c src/neighbor_table.c src/arp.c src/icmp.c src/pending_queue.c

all: test_ethernet test_mac_table test_switch test_vlan \
     test_ipv4 test_lpm test_ipv4_checksum test_neighbor test_l3

test_ethernet: src/ethernet.c tests/test_ethernet.c
	$(CC) $(CFLAGS) $^ -o $@

test_mac_table: src/mac_table.c tests/test_mac_table.c
	$(CC) $(CFLAGS) $^ -o $@

test_switch: $(SWITCH_SRCS) tests/test_switch.c
	$(CC) $(CFLAGS) $^ -o $@

test_vlan: $(SWITCH_SRCS) tests/test_vlan.c
	$(CC) $(CFLAGS) $^ -o $@

test_ipv4: src/ipv4.c tests/test_ipv4.c
	$(CC) $(CFLAGS) $^ -o $@

test_lpm: src/route_table.c tests/test_lpm.c
	$(CC) $(CFLAGS) $^ -o $@

test_ipv4_checksum: src/ipv4.c tests/test_ipv4_checksum.c
	$(CC) $(CFLAGS) $^ -o $@

test_neighbor: src/neighbor_table.c tests/test_neighbor_table.c
	$(CC) $(CFLAGS) $^ -o $@

test_l3: $(SWITCH_SRCS) tests/test_l3.c
	$(CC) $(CFLAGS) $^ -o $@

test: all
	./test_ethernet
	./test_mac_table
	./test_switch
	./test_vlan
	./test_ipv4
	./test_lpm
	./test_ipv4_checksum
	./test_neighbor
	./test_l3

clean:
	rm -f test_ethernet test_mac_table test_switch test_vlan \
	      test_ipv4 test_lpm test_ipv4_checksum test_neighbor \
	      test_l3 test_ipv4_san test_lpm_san \
	      test_neighbor_san network_switch

.PHONY: all test clean
