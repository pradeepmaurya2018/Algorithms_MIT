#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>
#include <rte_log.h>
#include <rte_lcore.h>
#include <rte_ring.h>

#include "config.h"
#include "port.h"
#include "ring.h"
#include "classifier.h"
#include "qos.h"
#include "stats.h"

#define RTE_LOGTYPE_MAIN RTE_LOGTYPE_USER1

volatile int running = 1;

static void signal_handler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM) {
        RTE_LOG(INFO, MAIN, "Signal %d — shutting down\n", sig);
        running = 0;
    }
}

static int rx_loop(__rte_unused void *arg)
{
    struct rte_mbuf *bufs[PF_BURST_SIZE];
    uint16_t nb_rx, nb_enq;

    RTE_LOG(INFO, MAIN, "RX loop on lcore %u\n", rte_lcore_id());

    while (running) {
        nb_rx = rte_eth_rx_burst(PF_PORT_ID, 0, bufs, PF_BURST_SIZE);
        if (unlikely(nb_rx == 0))
            continue;
        nb_enq = rte_ring_enqueue_burst(g_rx_ring,
                                        (void **)bufs, nb_rx, NULL);
        for (uint16_t i = nb_enq; i < nb_rx; i++)
            rte_pktmbuf_free(bufs[i]);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int ret = rte_eal_init(argc, argv);
    if (ret < 0)
        rte_exit(EXIT_FAILURE, "EAL init failed\n");

    signal(SIGINT,  signal_handler);
    signal(SIGTERM, signal_handler);

    if (rte_eth_dev_count_avail() == 0)
        rte_exit(EXIT_FAILURE, "No DPDK ports\n");

    struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create(
        PF_MBUF_POOL_NAME, PF_NUM_MBUFS, PF_MBUF_CACHE,
        PF_MBUF_PRIV_SZ, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (!mbuf_pool)
        rte_exit(EXIT_FAILURE, "mbuf pool failed\n");

    if (port_init(PF_PORT_ID, mbuf_pool) < 0)
        rte_exit(EXIT_FAILURE, "Port init failed\n");

    if (rings_init() < 0)
        rte_exit(EXIT_FAILURE, "Ring init failed\n");

    if (stats_init() < 0)
        rte_exit(EXIT_FAILURE, "Stats init failed\n");

    telemetry_init();

    if (rte_lcore_count() < 4)
        rte_exit(EXIT_FAILURE, "Need 4 lcores: use -l 0-3\n");

    rte_eal_remote_launch(rx_loop,         NULL, PF_LCORE_RX);
    rte_eal_remote_launch(classifier_loop, NULL, PF_LCORE_CLS);
    rte_eal_remote_launch(qos_loop,        NULL, PF_LCORE_QOS);

    RTE_LOG(INFO, MAIN, "Pipeline running — Ctrl+C to stop.\n");

    while (running) {
        sleep(4);
        port_stats_print(PF_PORT_ID);
        printf("  Rings — HI:%u MED:%u LO:%u BG:%u\n",
               rte_ring_count(g_tc_ring[TC_HI]),
               rte_ring_count(g_tc_ring[TC_MED]),
               rte_ring_count(g_tc_ring[TC_LO]),
               rte_ring_count(g_tc_ring[TC_BG]));
        stats_print_top(8);
    }

    rte_eal_mp_wait_lcore();
    rte_eth_dev_stop(PF_PORT_ID);
    rte_eth_dev_close(PF_PORT_ID);
    rings_free();
    stats_free();
    rte_mempool_free(mbuf_pool);
    rte_eal_cleanup();
    return 0;
}
