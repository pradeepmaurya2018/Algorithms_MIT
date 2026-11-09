#include <stdio.h>
#include <string.h>

#include "switch.h"
#include "ipv4.h"
#include "arp.h"
#include "icmp.h"

static switch_port_t *find_port(switch_t *sw, uint16_t port_id)
{
    if (sw == NULL)
        return NULL;

    for (size_t i = 0; i < sw->num_ports; i++) {
        if (sw->ports[i].port_id == port_id)
            return &sw->ports[i];
    }

    return NULL;
}


static router_interface_t *find_router_interface_by_mac(
    switch_t *sw,
    uint16_t vlan_id,
    const uint8_t mac[MAC_ADDR_LEN])
{
    if (sw == NULL || mac == NULL)
        return NULL;

    for (size_t i = 0; i < sw->num_router_interfaces; i++) {
        router_interface_t *rif = &sw->router_interfaces[i];

        if (rif->enabled &&
            rif->vlan_id == vlan_id &&
            memcmp(rif->mac, mac, MAC_ADDR_LEN) == 0)
            return rif;
    }

    return NULL;
}

static router_interface_t *find_router_interface_by_vlan(
    switch_t *sw,
    uint16_t vlan_id)
{
    if (sw == NULL)
        return NULL;

    for (size_t i = 0; i < sw->num_router_interfaces; i++) {
        router_interface_t *rif = &sw->router_interfaces[i];

        if (rif->enabled && rif->vlan_id == vlan_id)
            return rif;
    }

    return NULL;
}


static router_interface_t *find_router_interface_by_ip(
    switch_t *sw,
    uint16_t vlan_id,
    uint32_t ip_address)
{
    if (sw == NULL || ip_address == 0)
        return NULL;

    for (size_t i = 0; i < sw->num_router_interfaces; i++) {
        router_interface_t *rif = &sw->router_interfaces[i];

        if (rif->enabled &&
            rif->vlan_id == vlan_id &&
            rif->ip_address == ip_address)
            return rif;
    }

    return NULL;
}

static arp_resolution_t *find_arp_resolution(
    switch_t *sw,
    uint32_t next_hop_ip,
    uint16_t vlan_id)
{
    if (sw == NULL)
        return NULL;

    for (size_t i = 0; i < SWITCH_MAX_ARP_RESOLUTIONS; i++) {
        arp_resolution_t *entry = &sw->arp_resolutions[i];

        if (entry->valid &&
            entry->next_hop_ip == next_hop_ip &&
            entry->vlan_id == vlan_id)
            return entry;
    }

    return NULL;
}

static arp_resolution_t *start_arp_resolution(
    switch_t *sw,
    uint32_t next_hop_ip,
    uint16_t vlan_id,
    uint16_t egress_port,
    uint64_t now_ms)
{
    arp_resolution_t *existing =
        find_arp_resolution(sw, next_hop_ip, vlan_id);

    if (existing != NULL)
        return existing;

    for (size_t i = 0; i < SWITCH_MAX_ARP_RESOLUTIONS; i++) {
        arp_resolution_t *entry = &sw->arp_resolutions[i];

        if (entry->valid)
            continue;

        memset(entry, 0, sizeof(*entry));
        entry->valid = true;
        entry->next_hop_ip = next_hop_ip;
        entry->vlan_id = vlan_id;
        entry->egress_port = egress_port;
        entry->attempts = 1;
        entry->last_request_ms = now_ms;

        return entry;
    }

    return NULL;
}

static void clear_arp_resolution(
    switch_t *sw,
    uint32_t next_hop_ip,
    uint16_t vlan_id)
{
    arp_resolution_t *entry =
        find_arp_resolution(sw, next_hop_ip, vlan_id);

    if (entry != NULL)
        memset(entry, 0, sizeof(*entry));
}

static bool router_mac_is_valid(const uint8_t mac[MAC_ADDR_LEN])
{
    if (mac == NULL || (mac[0] & 0x01U) != 0)
        return false;

    for (size_t i = 0; i < MAC_ADDR_LEN; i++) {
        if (mac[i] != 0)
            return true;
    }

    return false;
}

static bool vlan_is_allowed(
    const switch_port_t *port,
    uint16_t vlan_id)
{
    if (port == NULL || vlan_id == 0 || vlan_id > VLAN_MAX_ID)
        return false;

    size_t byte_index = vlan_id / 8;
    uint8_t bit_mask = (uint8_t)(1U << (vlan_id % 8));

    return (port->vlan_bitmap[byte_index] & bit_mask) != 0;
}

static void vlan_allow(
    switch_port_t *port,
    uint16_t vlan_id)
{
    size_t byte_index = vlan_id / 8;
    uint8_t bit_mask = (uint8_t)(1U << (vlan_id % 8));

    port->vlan_bitmap[byte_index] |= bit_mask;
}

static bool mac_is_multicast(const uint8_t *mac)
{
    return (mac[0] & 0x01) != 0;
}

static bool mac_is_zero(const uint8_t *mac)
{
    for (size_t i = 0; i < MAC_ADDR_LEN; i++) {
        if (mac[i] != 0)
            return false;
    }

    return true;
}

void switch_init(switch_t *sw)
{
    if (sw == NULL)
        return;

    memset(sw, 0, sizeof(*sw));

    mac_table_init(&sw->mac_table);
    neighbor_table_init(&sw->neighbor_table);
    pending_queue_init(&sw->pending_queue);
}

void switch_destroy(switch_t *sw)
{
    if (sw == NULL)
        return;

    route_table_destroy(&sw->route_table);
}


int switch_add_router_interface(
    switch_t *sw,
    uint16_t vlan_id,
    uint32_t ip_address,
    const uint8_t mac[MAC_ADDR_LEN])
{
    if (sw == NULL || vlan_id == 0 || vlan_id > VLAN_MAX_ID ||
        ip_address == 0 || !router_mac_is_valid(mac))
        return -1;

    if (sw->num_router_interfaces >= SWITCH_MAX_ROUTER_INTERFACES)
        return -1;

    for (size_t i = 0; i < sw->num_router_interfaces; i++) {
        if (sw->router_interfaces[i].vlan_id == vlan_id)
            return -1;

        if (memcmp(sw->router_interfaces[i].mac,
                   mac, MAC_ADDR_LEN) == 0)
            return -1;
    }

    router_interface_t *rif =
        &sw->router_interfaces[sw->num_router_interfaces];

    memset(rif, 0, sizeof(*rif));

    rif->vlan_id = vlan_id;
    rif->ip_address = ip_address;
    memcpy(rif->mac, mac, MAC_ADDR_LEN);
    rif->enabled = true;

    sw->num_router_interfaces++;

    return 0;
}

int switch_add_route(
    switch_t *sw,
    uint32_t network,
    uint8_t prefix_length,
    uint32_t next_hop,
    uint16_t egress_port,
    uint16_t vlan_id)
{
    if (sw == NULL || vlan_id == 0 || vlan_id > VLAN_MAX_ID ||
        prefix_length > IPV4_PREFIX_MAX)
        return -1;

    /*
     * Initialize the route trie lazily. This avoids allocating
     * a route tree for switches that only perform L2 forwarding.
     */
    if (sw->route_table.root == NULL) {
        route_table_init(&sw->route_table);

        if (sw->route_table.root == NULL)
            return -1;
    }

    return route_table_insert(
        &sw->route_table,
        network,
        prefix_length,
        next_hop,
        egress_port,
        vlan_id);
}

int switch_add_neighbor(
    switch_t *sw,
    uint32_t ip_address,
    uint16_t vlan_id,
    uint16_t port_id,
    const uint8_t mac[MAC_ADDR_LEN])
{
    if (sw == NULL)
        return -1;

    switch_port_t *port = find_port(sw, port_id);

    if (port == NULL || !vlan_is_allowed(port, vlan_id))
        return -1;

    return neighbor_table_add(
        &sw->neighbor_table,
        ip_address,
        vlan_id,
        port_id,
        mac);
}

static switch_port_t *allocate_port(
    switch_t *sw,
    uint16_t port_id)
{
    if (sw == NULL || port_id == 0)
        return NULL;

    if (sw->num_ports >= SWITCH_MAX_PORTS)
        return NULL;

    if (find_port(sw, port_id) != NULL)
        return NULL;

    switch_port_t *port = &sw->ports[sw->num_ports];

    memset(port, 0, sizeof(*port));

    port->port_id = port_id;
    port->enabled = true;

    sw->num_ports++;

    return port;
}

int switch_add_access_port(
    switch_t *sw,
    uint16_t port_id,
    uint16_t pvid)
{
    if (pvid == 0 || pvid > VLAN_MAX_ID)
        return -1;

    switch_port_t *port = allocate_port(sw, port_id);

    if (port == NULL)
        return -1;

    port->mode = PORT_MODE_ACCESS;
    port->pvid = pvid;

    vlan_allow(port, pvid);

    return 0;
}

int switch_add_trunk_port(
    switch_t *sw,
    uint16_t port_id)
{
    switch_port_t *port = allocate_port(sw, port_id);

    if (port == NULL)
        return -1;

    port->mode = PORT_MODE_TRUNK;

    return 0;
}

int switch_port_allow_vlan(
    switch_t *sw,
    uint16_t port_id,
    uint16_t vlan_id)
{
    if (vlan_id == 0 || vlan_id > VLAN_MAX_ID)
        return -1;

    switch_port_t *port = find_port(sw, port_id);

    if (port == NULL || port->mode != PORT_MODE_TRUNK)
        return -1;

    vlan_allow(port, vlan_id);

    return 0;
}

int switch_add_port(
    switch_t *sw,
    uint16_t port_id,
    uint16_t vlan_id)
{
    return switch_add_access_port(sw, port_id, vlan_id);
}

int switch_set_port_state(
    switch_t *sw,
    uint16_t port_id,
    bool enabled)
{
    switch_port_t *port = find_port(sw, port_id);

    if (port == NULL)
        return -1;

    port->enabled = enabled;

    return 0;
}

/*
 * Access ingress:
 *   Untagged and priority-tagged frames use PVID.
 *
 * Trunk ingress:
 *   Requires a tagged frame with a nonzero, allowed VID.
 */
static bool classify_ingress(
    const switch_port_t *port,
    const ethernet_header_t *eth,
    uint16_t *vlan_id)
{
    if (port == NULL || eth == NULL || vlan_id == NULL)
        return false;

    if (port->mode == PORT_MODE_ACCESS) {

        if (eth->is_vlan && eth->vlan_id != 0)
            return false;

        *vlan_id = port->pvid;

        return true;
    }

    if (port->mode == PORT_MODE_TRUNK) {

        if (!eth->is_vlan || eth->vlan_id == 0)
            return false;

        if (!vlan_is_allowed(port, eth->vlan_id))
            return false;

        *vlan_id = eth->vlan_id;

        return true;
    }

    return false;
}

/*
 * Build the actual egress Ethernet frame.
 *
 * Access: remove an existing VLAN tag.
 * Trunk:  insert an 802.1Q VLAN tag.
 */
static bool build_egress_packet(
    const packet_t *input,
    const ethernet_header_t *eth,
    uint16_t vlan_id,
    const switch_port_t *egress,
    packet_t *output)
{
    if (input == NULL || eth == NULL ||
        egress == NULL || output == NULL)
        return false;

    if (egress->mode == PORT_MODE_ACCESS) {

        if (egress->pvid != vlan_id)
            return false;

        if (eth->is_vlan) {
            /*
             * Remove the 802.1Q header before transmitting
             * through an access port.
             */
            if (input->len < VLAN_HEADER_LEN)
                return false;

            *output = *input;

            memmove(
                output->data + 12,
                output->data + 16,
                input->len - 16
            );

            output->len -= 4;
        } else {
            *output = *input;
        }

        output->egress_port = egress->port_id;

        return true;
    }

    if (egress->mode == PORT_MODE_TRUNK) {

        if (!vlan_is_allowed(egress, vlan_id))
            return false;

        if (eth->is_vlan && eth->vlan_id != 0) {
            /* Already tagged with the correct VLAN. */
            if (eth->vlan_id != vlan_id)
                return false;

            *output = *input;
        } else {
            if (input->len < ETH_HEADER_LEN ||
                input->len > MAX_PACKET_SIZE - 4)
                return false;

            *output = *input;

            memmove(
                output->data + 16,
                output->data + 12,
                input->len - 12
            );

            /* TPID: IEEE 802.1Q */
            output->data[12] = 0x81;
            output->data[13] = 0x00;

            /* TCI: PCP=0, DEI=0, VID=vlan_id */
            output->data[14] = (uint8_t)(vlan_id >> 8);
            output->data[15] = (uint8_t)(vlan_id & 0xFF);

            output->len += 4;
        }

        output->egress_port = egress->port_id;

        return true;
    }

    return false;
}

static bool add_egress(
    switch_result_t *result,
    const packet_t *packet,
    const ethernet_header_t *eth,
    uint16_t vlan_id,
    const switch_port_t *port)
{
    if (result->egress_count >= SWITCH_MAX_EGRESS_PACKETS)
        return false;

    size_t index = result->egress_count;

    if (!build_egress_packet(
            packet,
            eth,
            vlan_id,
            port,
            &result->egress_packets[index]))
        return false;

    result->egress_ports[index] = port->port_id;
    result->egress_count++;

    return true;
}



static int switch_route_ipv4_packet(
    switch_t *sw,
    const packet_t *packet,
    const ethernet_header_t *eth,
    uint16_t ingress_vlan,
    uint64_t now_ms,
    switch_result_t *result);

static void switch_flush_pending_packets(
    switch_t *sw,
    uint32_t next_hop_ip,
    uint16_t vlan_id,
    uint64_t now_ms,
    switch_result_t *result);

/*
 * Return values:
 *   0: Continue with normal L2 processing.
 *   1: ARP packet handled locally.
 *  -1: Invalid ARP packet; drop.
 */
static int switch_process_arp_packet(
    switch_t *sw,
    const packet_t *packet,
    const ethernet_header_t *eth,
    uint16_t vlan_id,
    uint64_t now_ms,
    switch_result_t *result)
{
    if (eth->ether_type != ETH_P_ARP)
        return 0;

    arp_packet_t arp;

    if (arp_parse(packet, eth->l3_offset, &arp) != 0)
        return -1;

    /* The Ethernet and ARP sender addresses must agree. */
    if (memcmp(eth->src_mac, arp.sender_mac, MAC_ADDR_LEN) != 0)
        return -1;

    /*
     * Learn the sender mapping. Sender IP zero is used by
     * address-probe packets and must not enter the neighbor table.
     */
    if (arp.sender_ip != 0) {
        (void)neighbor_table_add(
            &sw->neighbor_table,
            arp.sender_ip,
            vlan_id,
            packet->ingress_port,
            arp.sender_mac);
    }

    if (arp.opcode == ARP_OPCODE_REQUEST) {
        router_interface_t *rif =
            find_router_interface_by_ip(sw, vlan_id, arp.target_ip);

        /* Not addressed to this router: let L2 flood it. */
        if (rif == NULL)
            return 0;

        switch_port_t *ingress =
            find_port(sw, packet->ingress_port);

        if (ingress == NULL || !ingress->enabled)
            return -1;

        packet_t reply;
        ethernet_header_t reply_eth;

        if (arp_build_reply(
                &reply,
                rif->mac,
                rif->ip_address,
                arp.sender_mac,
                arp.sender_ip,
                ingress->port_id) != 0)
            return -1;

        reply.ingress_port = ingress->port_id;

        if (ethernet_parse(&reply, &reply_eth) != 0)
            return -1;

        if (!build_egress_packet(
                &reply,
                &reply_eth,
                vlan_id,
                ingress,
                &result->egress_packets[0]))
            return -1;

        result->egress_ports[0] = ingress->port_id;
        result->egress_count = 1;
        result->vlan_id = vlan_id;
        result->action = SWITCH_ACTION_ARP_REPLY;

        return 1;
    }

    if (arp.opcode == ARP_OPCODE_REPLY) {
        /*
         * A reply addressed to one of our router interfaces
         * is control-plane traffic. It has already populated
         * the neighbor table above.
         */
        router_interface_t *rif =
            find_router_interface_by_ip(sw, vlan_id, arp.target_ip);

        if (rif != NULL &&
            memcmp(arp.target_mac, rif->mac, MAC_ADDR_LEN) == 0) {

            clear_arp_resolution(sw, arp.sender_ip, vlan_id);

            switch_flush_pending_packets(
                sw, arp.sender_ip, vlan_id, now_ms, result);

            if (result->egress_count == 0) {
                result->action = SWITCH_ACTION_ARP_REPLY;
                result->vlan_id = vlan_id;
            }

            return 1;
        }
    }

    return 0;
}

/*
 * Route an IPv4 packet addressed to one of this switch's
 * router-interface MAC addresses.
 *
 * A route with next_hop == 0 is treated as directly connected:
 * the IPv4 destination itself is used for neighbor lookup.
 *
 * A route with egress_port == 0 uses the port stored in the
 * neighbor entry. A nonzero route egress_port overrides it.
 */
static int switch_route_ipv4_packet(
    switch_t *sw,
    const packet_t *packet,
    const ethernet_header_t *eth,
    uint16_t ingress_vlan,
    uint64_t now_ms,
    switch_result_t *result)
{
    ipv4_header_t ip;

    if (eth->ether_type != ETH_P_IP)
        return 0;

    if (ipv4_parse(packet, eth->l3_offset, &ip) != 0)
        return 0;

    if (!ipv4_checksum_valid(packet, eth->l3_offset, &ip))
        return 0;

    route_entry_t route;

    if (!route_table_lookup(&sw->route_table, ip.dst_ip, &route))
        return 0;

    uint32_t neighbor_ip =
        route.next_hop != 0 ? route.next_hop : ip.dst_ip;

    uint8_t neighbor_mac[MAC_ADDR_LEN];
    uint16_t neighbor_port_id = 0;

    if (!neighbor_table_lookup(
            &sw->neighbor_table,
            neighbor_ip,
            route.vlan_id,
            neighbor_mac,
            &neighbor_port_id)) {

        /*
         * Neighbor resolution is required.
         * Find the router interface that owns the egress VLAN.
         */
        router_interface_t *arp_rif =
            find_router_interface_by_vlan(sw, route.vlan_id);

        if (arp_rif == NULL)
            return 0;

        /*
         * Select the ports on which the ARP request must leave.
         * A configured route port is preferred. Otherwise,
         * broadcast to enabled members of the egress VLAN.
         */
        switch_port_t *arp_ports[SWITCH_MAX_PORTS];
        size_t arp_port_count = 0;

        if (route.egress_port != 0) {
            switch_port_t *port =
                find_port(sw, route.egress_port);

            if (port != NULL && port->enabled &&
                vlan_is_allowed(port, route.vlan_id)) {
                arp_ports[arp_port_count++] = port;
            }
        } else {
            for (size_t i = 0; i < sw->num_ports; i++) {
                switch_port_t *port = &sw->ports[i];

                if (!port->enabled ||
                    port->port_id == packet->ingress_port ||
                    !vlan_is_allowed(port, route.vlan_id))
                    continue;

                arp_ports[arp_port_count++] = port;
            }
        }

        if (arp_port_count == 0)
            return 0;

        packet_t arp_request;
        ethernet_header_t arp_eth;

        if (arp_build_request(
                &arp_request,
                arp_rif->mac,
                arp_rif->ip_address,
                neighbor_ip,
                arp_ports[0]->port_id) != 0)
            return 0;

        if (ethernet_parse(&arp_request, &arp_eth) != 0)
            return 0;

        /*
         * Buffer the original IPv4 packet.
         * Multiple packets for the same unresolved neighbor
         * share one outstanding ARP request.
         */
        bool request_already_pending =
            pending_queue_contains(
                &sw->pending_queue,
                neighbor_ip,
                route.vlan_id);

        if (!request_already_pending) {
            if (start_arp_resolution(
                    sw,
                    neighbor_ip,
                    route.vlan_id,
                    route.egress_port,
                    now_ms) == NULL)
                return 0;
        }

        if (pending_queue_enqueue(
                &sw->pending_queue,
                packet,
                neighbor_ip,
                route.vlan_id,
                now_ms) != 0) {

            if (!request_already_pending)
                clear_arp_resolution(
                    sw, neighbor_ip, route.vlan_id);

            return 0;
        }

        result->vlan_id = route.vlan_id;

        if (request_already_pending) {
            result->action = SWITCH_ACTION_ARP_PENDING;
            return 0;
        }

        /*
         * First packet for this unresolved next hop:
         * transmit the ARP request.
         */
        for (size_t i = 0; i < arp_port_count; i++) {
            (void)add_egress(
                result,
                &arp_request,
                &arp_eth,
                route.vlan_id,
                arp_ports[i]);
        }

        if (result->egress_count == 0) {
            result->action = SWITCH_ACTION_DROP;
            return 0;
        }

        result->action = SWITCH_ACTION_ARP_REQUEST;
        return 0;
    }

    uint16_t egress_port_id =
        route.egress_port != 0 ? route.egress_port : neighbor_port_id;

    switch_port_t *egress = find_port(sw, egress_port_id);

    if (egress == NULL || !egress->enabled ||
        !vlan_is_allowed(egress, route.vlan_id))
        return 0;

    router_interface_t *egress_rif =
        find_router_interface_by_vlan(sw, route.vlan_id);

    if (egress_rif == NULL)
        return 0;

    /*
     * Validate and update TTL/checksum before modifying
     * the Ethernet header.
     */
    packet_t routed = *packet;

    if (ipv4_decrement_ttl(
            &routed, eth->l3_offset, &ip) != 0)
        return 0;

    /*
     * Remove the ingress VLAN tag, if present.
     * Egress VLAN encapsulation is applied separately below.
     */
    if (eth->is_vlan) {
        if (routed.len < VLAN_HEADER_LEN)
            return 0;

        memmove(
            routed.data + 12,
            routed.data + 16,
            routed.len - 16);

        routed.len -= 4;
    }

    /*
     * Rewrite Ethernet addresses:
     * destination = next-hop neighbor MAC
     * source      = egress router-interface MAC
     */
    memcpy(routed.data, neighbor_mac, MAC_ADDR_LEN);
    memcpy(
        routed.data + MAC_ADDR_LEN,
        egress_rif->mac,
        MAC_ADDR_LEN);

    ethernet_header_t routed_eth;

    if (ethernet_parse(&routed, &routed_eth) != 0)
        return 0;

    if (!build_egress_packet(
            &routed,
            &routed_eth,
            route.vlan_id,
            egress,
            &result->egress_packets[0]))
        return 0;

    result->egress_ports[0] = egress->port_id;
    result->egress_count = 1;
    result->vlan_id = route.vlan_id;
    result->action = SWITCH_ACTION_ROUTED;

    (void)ingress_vlan;

    return 0;
}

/*
 * An ARP reply has resolved next_hop_ip.
 * Route every buffered packet waiting for that neighbor.
 */
static void switch_flush_pending_packets(
    switch_t *sw,
    uint32_t next_hop_ip,
    uint16_t vlan_id,
    uint64_t now_ms,
    switch_result_t *result)
{
    if (sw == NULL || result == NULL)
        return;

    pending_packet_t pending;

    while (pending_queue_pop_match(
               &sw->pending_queue,
               next_hop_ip,
               vlan_id,
               &pending)) {

        ethernet_header_t eth;

        if (ethernet_parse(&pending.packet, &eth) != 0)
            continue;

        switch_result_t routed_result;
        memset(&routed_result, 0, sizeof(routed_result));

        (void)switch_route_ipv4_packet(
            sw,
            &pending.packet,
            &eth,
            pending.vlan_id,
            now_ms,
            &routed_result);

        if (routed_result.action != SWITCH_ACTION_ROUTED)
            continue;

        for (size_t i = 0; i < routed_result.egress_count; i++) {
            if (result->egress_count >= SWITCH_MAX_EGRESS_PACKETS)
                return;

            size_t index = result->egress_count++;

            result->egress_ports[index] =
                routed_result.egress_ports[i];

            result->egress_packets[index] =
                routed_result.egress_packets[i];
        }

        result->vlan_id = routed_result.vlan_id;
        result->action = SWITCH_ACTION_ROUTED;
    }
}

/*
 * Process IPv4 packets addressed to a local router interface.
 *
 * Return:
 *   0: Not a local IPv4 packet; continue normal routing.
 *   1: Packet consumed locally.
 */
static int switch_process_local_ipv4_packet(
    const packet_t *packet,
    const ethernet_header_t *eth,
    const router_interface_t *rif,
    switch_port_t *ingress,
    uint16_t vlan_id,
    switch_result_t *result)
{
    if (packet == NULL || eth == NULL || rif == NULL ||
        ingress == NULL || result == NULL)
        return 1;

    if (eth->ether_type != ETH_P_IP)
        return 0;

    ipv4_header_t ip;

    if (ipv4_parse(packet, eth->l3_offset, &ip) != 0)
        return 1;

    if (!ipv4_checksum_valid(packet, eth->l3_offset, &ip))
        return 1;

    /* This packet is destined for another host, not this router. */
    if (ip.dst_ip != rif->ip_address)
        return 0;

    /* The destination is local, but only ICMP is implemented here. */
    if (ip.protocol != ICMP_PROTOCOL)
        return 1;

    packet_t reply;

    if (icmp_build_echo_reply(
            &reply,
            packet,
            eth->l3_offset,
            &ip) != 0)
        return 1;

    /*
     * Ethernet reply:
     *   Destination = original sender MAC
     *   Source      = local router-interface MAC
     */
    memcpy(reply.data, eth->src_mac, MAC_ADDR_LEN);
    memcpy(reply.data + MAC_ADDR_LEN, rif->mac, MAC_ADDR_LEN);

    ethernet_header_t reply_eth;

    if (ethernet_parse(&reply, &reply_eth) != 0)
        return 1;

    if (!build_egress_packet(
            &reply,
            &reply_eth,
            vlan_id,
            ingress,
            &result->egress_packets[0]))
        return 1;

    result->egress_ports[0] = ingress->port_id;
    result->egress_count = 1;
    result->vlan_id = vlan_id;
    result->action = SWITCH_ACTION_ICMP_REPLY;

    return 1;
}

int switch_process_packet(
    switch_t *sw,
    const packet_t *packet,
    uint64_t now_ms,
    switch_result_t *result)
{
    if (sw == NULL || packet == NULL || result == NULL)
        return -1;

    memset(result, 0, sizeof(*result));

    result->action = SWITCH_ACTION_DROP;

    (void)pending_queue_age(
        &sw->pending_queue,
        now_ms,
        SWITCH_PENDING_TIMEOUT_MS);

    switch_port_t *ingress = find_port(sw, packet->ingress_port);

    if (ingress == NULL || !ingress->enabled)
        return 0;

    ethernet_header_t eth;

    if (ethernet_parse(packet, &eth) != 0)
        return 0;

    uint16_t vlan_id;

    if (!classify_ingress(ingress, &eth, &vlan_id))
        return 0;

    result->vlan_id = vlan_id;

    /* Learn source MAC. */
    if (!mac_is_multicast(eth.src_mac) &&
        !mac_is_zero(eth.src_mac)) {

        if (mac_table_learn(
                &sw->mac_table,
                eth.src_mac,
                vlan_id,
                packet->ingress_port,
                now_ms) != 0)
            return 0;
    }

    int arp_result = switch_process_arp_packet(
        sw, packet, &eth, vlan_id, now_ms, result);

    if (arp_result != 0)
        return 0;

    /*
     * If the destination MAC belongs to a router interface
     * on this VLAN, use the IPv4 routing path.
     */
    router_interface_t *ingress_rif =
        find_router_interface_by_mac(sw, vlan_id, eth.dst_mac);

    if (ingress_rif != NULL) {

        int local_result = switch_process_local_ipv4_packet(
            packet,
            &eth,
            ingress_rif,
            ingress,
            vlan_id,
            result);

        if (local_result != 0)
            return 0;

        return switch_route_ipv4_packet(
            sw, packet, &eth, vlan_id, now_ms, result);
    }

    bool flood = mac_is_multicast(eth.dst_mac);
    uint16_t destination_port = 0;

    if (!flood) {

        if (mac_table_lookup(
                &sw->mac_table,
                eth.dst_mac,
                vlan_id,
                &destination_port)) {

            if (destination_port == packet->ingress_port) {
                result->action = SWITCH_ACTION_FILTERED;
                return 0;
            }

            switch_port_t *egress = find_port(sw, destination_port);

            if (egress == NULL || !egress->enabled ||
                !vlan_is_allowed(egress, vlan_id)) {
                return 0;
            }

            if (!add_egress(result, packet, &eth, vlan_id, egress))
                return 0;

            result->action = SWITCH_ACTION_UNICAST;

            return 0;
        }

        flood = true;
    }

    /* Broadcast, multicast and unknown unicast flooding. */
    if (flood) {

        for (size_t i = 0; i < sw->num_ports; i++) {

            switch_port_t *port = &sw->ports[i];

            if (!port->enabled)
                continue;

            if (port->port_id == packet->ingress_port)
                continue;

            if (!vlan_is_allowed(port, vlan_id))
                continue;

            (void)add_egress(result, packet, &eth, vlan_id, port);
        }

        result->action = SWITCH_ACTION_FLOOD;
    }

    return 0;
}

/*
 * Timer-driven control-plane processing.
 *
 * Call periodically from the simulator's main event loop.
 * Generates ARP retransmissions and expires unresolved packets.
 */
int switch_tick(
    switch_t *sw,
    uint64_t now_ms,
    switch_result_t *result)
{
    if (sw == NULL || result == NULL)
        return -1;

    memset(result, 0, sizeof(*result));
    result->action = SWITCH_ACTION_DROP;

    /* Expire packets that have waited too long for ARP resolution. */
    (void)pending_queue_age(
        &sw->pending_queue,
        now_ms,
        SWITCH_PENDING_TIMEOUT_MS);

    for (size_t i = 0; i < SWITCH_MAX_ARP_RESOLUTIONS; i++) {
        arp_resolution_t *resolution = &sw->arp_resolutions[i];

        if (!resolution->valid)
            continue;

        /* No buffered packets remain for this resolution. */
        if (!pending_queue_contains(
                &sw->pending_queue,
                resolution->next_hop_ip,
                resolution->vlan_id)) {
            memset(resolution, 0, sizeof(*resolution));
            continue;
        }

        /* Do not retry more than the configured number of times. */
        if (resolution->attempts >= SWITCH_ARP_MAX_ATTEMPTS)
            continue;

        if (now_ms < resolution->last_request_ms)
            continue;

        if (now_ms - resolution->last_request_ms <
            SWITCH_ARP_RETRY_INTERVAL_MS)
            continue;

        router_interface_t *rif =
            find_router_interface_by_vlan(sw, resolution->vlan_id);

        if (rif == NULL)
            continue;

        switch_port_t *arp_ports[SWITCH_MAX_PORTS];
        size_t arp_port_count = 0;

        if (resolution->egress_port != 0) {
            switch_port_t *port =
                find_port(sw, resolution->egress_port);

            if (port != NULL && port->enabled &&
                vlan_is_allowed(port, resolution->vlan_id))
                arp_ports[arp_port_count++] = port;
        } else {
            for (size_t j = 0; j < sw->num_ports; j++) {
                switch_port_t *port = &sw->ports[j];

                if (port->enabled &&
                    vlan_is_allowed(port, resolution->vlan_id))
                    arp_ports[arp_port_count++] = port;
            }
        }

        if (arp_port_count == 0)
            continue;

        packet_t request;
        ethernet_header_t request_eth;

        if (arp_build_request(
                &request,
                rif->mac,
                rif->ip_address,
                resolution->next_hop_ip,
                arp_ports[0]->port_id) != 0)
            continue;

        if (ethernet_parse(&request, &request_eth) != 0)
            continue;

        size_t previous_count = result->egress_count;

        for (size_t j = 0; j < arp_port_count; j++) {
            if (result->egress_count >= SWITCH_MAX_EGRESS_PACKETS)
                break;

            (void)add_egress(
                result,
                &request,
                &request_eth,
                resolution->vlan_id,
                arp_ports[j]);
        }

        /* Count an attempt only if the request was actually emitted. */
        if (result->egress_count > previous_count) {
            resolution->attempts++;
            resolution->last_request_ms = now_ms;

            result->action = SWITCH_ACTION_ARP_REQUEST;
            result->vlan_id = resolution->vlan_id;
        }

        if (result->egress_count >= SWITCH_MAX_EGRESS_PACKETS)
            break;
    }

    return 0;
}

void switch_print_result(const switch_result_t *result)
{
    if (result == NULL)
        return;

    const char *action = "UNKNOWN";

    switch (result->action) {
    case SWITCH_ACTION_DROP:
        action = "DROP";
        break;
    case SWITCH_ACTION_UNICAST:
        action = "UNICAST";
        break;
    case SWITCH_ACTION_FLOOD:
        action = "FLOOD";
        break;
    case SWITCH_ACTION_FILTERED:
        action = "FILTERED";
        break;
    case SWITCH_ACTION_ROUTED:
        action = "ROUTED";
        break;
    case SWITCH_ACTION_ARP_REPLY:
        action = "ARP_REPLY";
        break;
    case SWITCH_ACTION_ARP_REQUEST:
        action = "ARP_REQUEST";
        break;
    case SWITCH_ACTION_ARP_PENDING:
        action = "ARP_PENDING";
        break;
    case SWITCH_ACTION_ICMP_REPLY:
        action = "ICMP_REPLY";
        break;
    }

    printf("\nSwitch forwarding decision\n");
    printf("--------------------------\n");
    printf("Action: %s\n", action);
    printf("VLAN: %u\n", result->vlan_id);
    printf("Egress ports: ");

    if (result->egress_count == 0)
        printf("none");

    for (size_t i = 0; i < result->egress_count; i++)
        printf("%u ", result->egress_ports[i]);

    printf("\n");
}
