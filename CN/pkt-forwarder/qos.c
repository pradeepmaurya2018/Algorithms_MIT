#include <stdint.h>
#include <unistd.h>
#include <rte_mbuf.h>
#include <rte_ethdev.h>
#include <rte_cycles.h>
#include <rte_log.h>
#include <rte_ring.h>
#include <rte_prefetch.h>

#include "config.h"
#include "ring.h"
#include "qos.h"
#include "stats.h"

#define RTE_LOGTYPE_QOS RTE_LOGTYPE_USER4

extern volatile int running;

/*
 * QoS policy per traffic class:
 *   HI  — 400 Mbps, 64 KB burst  (voice/control — near line rate)
 *   MED — 300 Mbps, 32 KB burst  (interactive)
 *   LO  — 200 Mbps, 16 KB burst  (bulk)
 *   BG  — 100 Mbps,  8 KB burst  (background — rate-limited hard)
 *
 * Total = 1 Gbps = RTL8111 line rate. In practice the NIC caps it.
 */
const qos_policy_t g_qos_policy[TC_MAX] = {
    [TC_HI]  = { .rate_mbps = 400, .burst_bytes = 65536 },
    [TC_MED] = { .rate_mbps = 300, .burst_bytes = 32768 },
    [TC_LO]  = { .rate_mbps = 200, .burst_bytes = 16384 },
    [TC_BG]  = { .rate_mbps = 100, .burst_bytes =  8192 },
};

/* ------------------------------------------------------------------ */
/* Token bucket                                                         */
/* ------------------------------------------------------------------ */

void tb_init(token_bucket_t *tb, uint32_t rate_mbps, uint32_t burst_bytes)
{
    uint64_t hz = rte_get_tsc_hz();

    tb->burst_bytes      = burst_bytes;
    tb->tokens           = burst_bytes;   /* start full */
    tb->last_refill_tsc  = rte_rdtsc();

    /*
     * rate_bytes_per_ns = (rate_mbps * 1e6 / 8) / 1e9
     *                   = rate_mbps / 8000
     *
     * We work in TSC ticks instead of nanoseconds for precision:
     *   rate_bytes_per_tick = (rate_mbps * 1e6 / 8) / hz
     *
     * Store as bytes-per-hz scaled by 2^20 to avoid float:
     *   tokens_per_tick * 2^20 = (rate_mbps * 1e6 / 8) * 2^20 / hz
     */
    tb->rate_bytes_per_ns = ((uint64_t)rate_mbps * 1000000ULL) /
                             (hz / 1000ULL) / 8000ULL;

    /* Minimum 1 to avoid starvation on slow clocks */
    if (tb->rate_bytes_per_ns == 0)
        tb->rate_bytes_per_ns = 1;

    RTE_LOG(INFO, QOS,
            "TB init: rate=%u Mbps burst=%u B rate_B/us=%"PRIu64"\n",
            rate_mbps, burst_bytes, tb->rate_bytes_per_ns);
}

/*
 * Refill tokens based on elapsed TSC ticks, then try to consume.
 * Returns 0 if packet is allowed, -1 if dropped.
 */
int tb_consume(token_bucket_t *tb, uint32_t pkt_bytes)
{
    uint64_t now     = rte_rdtsc();
    uint64_t elapsed = now - tb->last_refill_tsc;  /* in ticks */
    uint64_t hz      = rte_get_tsc_hz();

    /* Refill: tokens += elapsed_us * rate_bytes_per_us */
    uint64_t new_tokens = (elapsed * tb->rate_bytes_per_ns) /
                          (hz / 1000000ULL);

    tb->tokens += new_tokens;
    if (tb->tokens > tb->burst_bytes)
        tb->tokens = tb->burst_bytes;   /* cap at burst */

    tb->last_refill_tsc = now;

    if (tb->tokens >= pkt_bytes) {
        tb->tokens -= pkt_bytes;
        return 0;   /* allowed */
    }
    return -1;      /* drop */
}

/* ------------------------------------------------------------------ */
/* Scheduler: strict priority + token bucket per class                 */
/* ------------------------------------------------------------------ */

/*
 * Weighted Round Robin across TC rings, gated by token buckets.
 * Priority order: HI → MED → LO → BG
 * Each iteration we try HI first; only move to lower class if HI empty.
 */
int qos_loop(__rte_unused void *arg)
{
    struct rte_mbuf *bufs[PF_BURST_SIZE];
    struct rte_mbuf *allowed[PF_BURST_SIZE];
    token_bucket_t   tb[TC_MAX];
    uint16_t nb_rx, nb_allowed, nb_tx;
    uint32_t pkt_len;

    RTE_LOG(INFO, QOS, "QoS scheduler on lcore %u\n", rte_lcore_id());

    /* Init one token bucket per class */
    for (int i = 0; i < TC_MAX; i++)
        tb_init(&tb[i], g_qos_policy[i].rate_mbps,
                        g_qos_policy[i].burst_bytes);

    while (running) {

        /* Strict priority: service HI first, fall through only if empty */
        for (int tc = TC_HI; tc < TC_MAX; tc++) {

            nb_rx = rte_ring_dequeue_burst(g_tc_ring[tc],
                                           (void **)bufs,
                                           PF_BURST_SIZE, NULL);
            if (nb_rx == 0)
                continue;

            /* Prefetch first few packet headers */
            for (uint16_t i = 0; i < RTE_MIN(nb_rx, 4u); i++)
                rte_prefetch0(rte_pktmbuf_mtod(bufs[i], void *));

            nb_allowed = 0;
            for (uint16_t i = 0; i < nb_rx; i++) {
                pkt_len = rte_pktmbuf_pkt_len(bufs[i]);

                if (tb_consume(&tb[tc], pkt_len) == 0) {
                    allowed[nb_allowed++] = bufs[i];
                } else {
                    g_tc_drops[tc].tb_drops++;
                    rte_pktmbuf_free(bufs[i]);
                    RTE_LOG(DEBUG, QOS,
                            "TC%d: token bucket drop (%u B)\n", tc, pkt_len);
                }
            }

            if (nb_allowed == 0)
                continue;

            /* TX burst — best effort, free unsent */
            nb_tx = rte_eth_tx_burst(PF_PORT_ID, 0, allowed, nb_allowed);
            for (uint16_t i = nb_tx; i < nb_allowed; i++)
                rte_pktmbuf_free(allowed[i]);

            /* Once we've serviced a higher-priority class this iteration,
             * restart from HI next loop to maintain strict priority */
            if (tc < TC_LO)
                break;
        }
    }

    return 0;
}
