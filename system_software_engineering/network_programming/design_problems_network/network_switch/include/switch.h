#ifndef SWITCH_H
#define SWITCH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "packet.h"
#include "ethernet.h"
#include "mac_table.h"
#include "route_table.h"
#include "neighbor_table.h"
#include "pending_queue.h"

#define SWITCH_MAX_PORTS 8
#define SWITCH_MAX_EGRESS_PACKETS PENDING_QUEUE_CAPACITY
#define SWITCH_PENDING_TIMEOUT_MS 30000ULL
#define SWITCH_ARP_RETRY_INTERVAL_MS 1000ULL
#define SWITCH_ARP_MAX_ATTEMPTS 3
#define SWITCH_MAX_ARP_RESOLUTIONS 64
#define SWITCH_MAX_ROUTER_INTERFACES 16
#define VLAN_MAX_ID 4094
#define VLAN_BITMAP_SIZE 512

typedef enum {
    PORT_MODE_ACCESS = 0,
    PORT_MODE_TRUNK
} port_mode_t;

typedef struct {
    uint16_t port_id;
    port_mode_t mode;
    uint16_t pvid;
    bool enabled;
    uint8_t vlan_bitmap[VLAN_BITMAP_SIZE];
} switch_port_t;

typedef struct {
    uint16_t vlan_id;
    uint32_t ip_address;
    uint8_t mac[MAC_ADDR_LEN];
    bool enabled;
} router_interface_t;

typedef struct {
    bool valid;
    uint32_t next_hop_ip;
    uint16_t vlan_id;
    uint16_t egress_port;
    uint8_t attempts;
    uint64_t last_request_ms;
} arp_resolution_t;

typedef struct {
    switch_port_t ports[SWITCH_MAX_PORTS];
    mac_table_t mac_table;

    route_table_t route_table;
    neighbor_table_t neighbor_table;
    pending_queue_t pending_queue;
    arp_resolution_t arp_resolutions[SWITCH_MAX_ARP_RESOLUTIONS];

    router_interface_t router_interfaces[SWITCH_MAX_ROUTER_INTERFACES];
    size_t num_router_interfaces;

    size_t num_ports;
} switch_t;

typedef enum {
    SWITCH_ACTION_DROP = 0,
    SWITCH_ACTION_UNICAST,
    SWITCH_ACTION_FLOOD,
    SWITCH_ACTION_FILTERED,
    SWITCH_ACTION_ROUTED,
    SWITCH_ACTION_ARP_REPLY,
    SWITCH_ACTION_ARP_REQUEST,
    SWITCH_ACTION_ARP_PENDING,
    SWITCH_ACTION_ICMP_REPLY
} switch_action_t;

typedef struct {
    switch_action_t action;
    uint16_t vlan_id;
    uint16_t egress_ports[SWITCH_MAX_EGRESS_PACKETS];
    packet_t egress_packets[SWITCH_MAX_EGRESS_PACKETS];
    size_t egress_count;
} switch_result_t;

void switch_init(switch_t *sw);
void switch_destroy(switch_t *sw);

int switch_add_access_port(
    switch_t *sw,
    uint16_t port_id,
    uint16_t pvid
);

int switch_add_trunk_port(
    switch_t *sw,
    uint16_t port_id
);

int switch_port_allow_vlan(
    switch_t *sw,
    uint16_t port_id,
    uint16_t vlan_id
);

int switch_add_port(
    switch_t *sw,
    uint16_t port_id,
    uint16_t vlan_id
);

int switch_set_port_state(
    switch_t *sw,
    uint16_t port_id,
    bool enabled
);

int switch_add_router_interface(
    switch_t *sw,
    uint16_t vlan_id,
    uint32_t ip_address,
    const uint8_t mac[MAC_ADDR_LEN]
);

int switch_add_route(
    switch_t *sw,
    uint32_t network,
    uint8_t prefix_length,
    uint32_t next_hop,
    uint16_t egress_port,
    uint16_t vlan_id
);

int switch_add_neighbor(
    switch_t *sw,
    uint32_t ip_address,
    uint16_t vlan_id,
    uint16_t port_id,
    const uint8_t mac[MAC_ADDR_LEN]
);

int switch_process_packet(
    switch_t *sw,
    const packet_t *packet,
    uint64_t now_ms,
    switch_result_t *result
);

int switch_tick(switch_t *sw, uint64_t now_ms, switch_result_t *result);

void switch_print_result(const switch_result_t *result);

#endif
