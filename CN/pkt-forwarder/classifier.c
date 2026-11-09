#include <stdint.h>
#include <rte_mbuf.h>
#include <rte_ether.h>
#include <rte_ip.h>
#include <rte_tcp.h>
#include <rte_udp.h>
#include <rte_log.h>
#include <rte_ring.h>

#include "config.h"
#include "ring.h"
#include "classifier.h"
#include "stats.h"

#define RTE_LOGTYPE_CLASS RTE_LOGTYPE_USER3

extern volatile int running;

static inline int classify_pkt(struct rte_mbuf *m, flow_key_t *key)
{
    struct rte_ether_hdr *eth;
    struct rte_ipv4_hdr  *ip;

    eth = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);

    if (unlikely(rte_be_to_cpu_16(eth->ether_type) != RTE_ETHER_TYPE_IPV4))
        return TC_BG;

    ip = (struct rte_ipv4_hdr *)(eth + 1);

    key->src_ip  = ip->src_addr;
    key->dst_ip  = ip->dst_addr;
    key->proto   = ip->next_proto_id;
    key->src_port = 0;
    key->dst_port = 0;

    if (ip->next_proto_id == IPPROTO_ICMP)
        return TC_HI;

    if (ip->next_proto_id == IPPROTO_UDP) {
        struct rte_udp_hdr *udp = (struct rte_udp_hdr *)
            ((uint8_t *)ip + (ip->ihl * 4));
        key->src_port = udp->src_port;
        key->dst_port = udp->dst_port;
        if (rte_be_to_cpu_16(udp->dst_port) == 5060 ||
            rte_be_to_cpu_16(udp->src_port) == 5060)
            return TC_HI;
        return TC_LO;
    }

    if (ip->next_proto_id == IPPROTO_TCP) {
        struct rte_tcp_hdr *tcp = (struct rte_tcp_hdr *)
            ((uint8_t *)ip + (ip->ihl * 4));
        key->src_port = tcp->src_port;
        key->dst_port = tcp->dst_port;
        uint16_t dp = rte_be_to_cpu_16(tcp->dst_port);
        uint16_t sp = rte_be_to_cpu_16(tcp->src_port);
        if (dp == 22 || sp == 22 || dp == 23 || sp == 23)
            return TC_MED;
        if (dp == 80 || sp == 80 || dp == 443 || sp == 443)
            return TC_MED;
        if (rte_pktmbuf_data_len(m) > 512)
            return TC_LO;
    }

    return TC_BG;
}

int classifier_loop(__rte_unused void *arg)
{
    struct rte_mbuf *bufs[PF_BURST_SIZE];
    flow_key_t       key;
    flow_stats_t    *fs;
    uint16_t nb_rx;
    int tc, nb_tx;

    RTE_LOG(INFO, CLASS, "Classifier on lcore %u\n", rte_lcore_id());

    while (running) {
        nb_rx = rte_ring_dequeue_burst(g_rx_ring,
                                       (void **)bufs,
                                       PF_BURST_SIZE, NULL);
        if (unlikely(nb_rx == 0))
            continue;

        for (uint16_t i = 0; i < nb_rx; i++) {
            memset(&key, 0, sizeof(key));
            tc = classify_pkt(bufs[i], &key);

            /* Update per-flow stats */
            fs = stats_get_or_create(&key, (uint8_t)tc);
            if (likely(fs != NULL)) {
                fs->rx_pkts++;
                fs->rx_bytes += rte_pktmbuf_pkt_len(bufs[i]);
            }

            nb_tx = rte_ring_enqueue(g_tc_ring[tc], bufs[i]);
            if (unlikely(nb_tx != 0)) {
                g_tc_drops[tc].ring_drops++;
                if (fs) fs->drop_pkts++;
                rte_pktmbuf_free(bufs[i]);
            }
        }
    }
    return 0;
}
