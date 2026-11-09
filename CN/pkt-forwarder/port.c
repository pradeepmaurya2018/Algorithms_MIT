#include <stdint.h>
#include <rte_ethdev.h>
#include <rte_log.h>
#include "config.h"
#include "port.h"

#define RTE_LOGTYPE_PORT RTE_LOGTYPE_USER1

/* Ethernet port configuration — conservative defaults for RTL8111 */
static const struct rte_eth_conf port_conf = {
    .rxmode = {
        .mq_mode = RTE_ETH_MQ_RX_NONE,  /* no RSS yet — single queue */
    },
    .txmode = {
        .mq_mode = RTE_ETH_MQ_TX_NONE,
    },
};

int port_init(uint16_t port_id, struct rte_mempool *mbuf_pool)
{
    struct rte_eth_dev_info dev_info;
    int ret;

    if (!rte_eth_dev_is_valid_port(port_id)) {
        RTE_LOG(ERR, PORT, "Invalid port %u\n", port_id);
        return -1;
    }

    ret = rte_eth_dev_info_get(port_id, &dev_info);
    if (ret < 0) {
        RTE_LOG(ERR, PORT, "Cannot get device info for port %u: %s\n",
                port_id, rte_strerror(-ret));
        return ret;
    }

    RTE_LOG(INFO, PORT, "Port %u: driver=%s max_rx_queues=%u max_tx_queues=%u\n",
            port_id, dev_info.driver_name,
            dev_info.max_rx_queues, dev_info.max_tx_queues);

    /* Configure the Ethernet device */
    ret = rte_eth_dev_configure(port_id,
                                PF_NUM_RX_QUEUES,
                                PF_NUM_TX_QUEUES,
                                &port_conf);
    if (ret < 0) {
        RTE_LOG(ERR, PORT, "rte_eth_dev_configure failed: %s\n",
                rte_strerror(-ret));
        return ret;
    }

    /* Adjust ring sizes to what the driver actually supports */
    uint16_t rx_size = PF_RX_RING_SIZE;
    uint16_t tx_size = PF_TX_RING_SIZE;
    ret = rte_eth_dev_adjust_nb_rx_tx_desc(port_id, &rx_size, &tx_size);
    if (ret < 0) {
        RTE_LOG(ERR, PORT, "rte_eth_dev_adjust_nb_rx_tx_desc failed: %s\n",
                rte_strerror(-ret));
        return ret;
    }
    RTE_LOG(INFO, PORT, "Port %u: rx_ring=%u tx_ring=%u\n",
            port_id, rx_size, tx_size);

    /* Set up 1 RX queue on lcore socket */
    ret = rte_eth_rx_queue_setup(port_id, 0, rx_size,
                                 rte_eth_dev_socket_id(port_id),
                                 NULL,   /* default rx conf */
                                 mbuf_pool);
    if (ret < 0) {
        RTE_LOG(ERR, PORT, "rte_eth_rx_queue_setup failed: %s\n",
                rte_strerror(-ret));
        return ret;
    }

    /* Set up 1 TX queue */
    struct rte_eth_txconf txconf = dev_info.default_txconf;
    txconf.offloads = port_conf.txmode.offloads;
    ret = rte_eth_tx_queue_setup(port_id, 0, tx_size,
                                 rte_eth_dev_socket_id(port_id),
                                 &txconf);
    if (ret < 0) {
        RTE_LOG(ERR, PORT, "rte_eth_tx_queue_setup failed: %s\n",
                rte_strerror(-ret));
        return ret;
    }

    /* Start the port */
    ret = rte_eth_dev_start(port_id);
    if (ret < 0) {
        RTE_LOG(ERR, PORT, "rte_eth_dev_start failed: %s\n",
                rte_strerror(-ret));
        return ret;
    }

    /* Enable promiscuous mode — capture all packets for testing */
    rte_eth_promiscuous_enable(port_id);

    struct rte_ether_addr addr;
    rte_eth_macaddr_get(port_id, &addr);
    RTE_LOG(INFO, PORT,
            "Port %u started. MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
            port_id,
            addr.addr_bytes[0], addr.addr_bytes[1],
            addr.addr_bytes[2], addr.addr_bytes[3],
            addr.addr_bytes[4], addr.addr_bytes[5]);

    return 0;
}

void port_stats_print(uint16_t port_id)
{
    struct rte_eth_stats stats;
    rte_eth_stats_get(port_id, &stats);

    printf("\n--- Port %u stats ---\n", port_id);
    printf("  RX packets : %"PRIu64"\n", stats.ipackets);
    printf("  TX packets : %"PRIu64"\n", stats.opackets);
    printf("  RX bytes   : %"PRIu64"\n", stats.ibytes);
    printf("  TX bytes   : %"PRIu64"\n", stats.obytes);
    printf("  RX missed  : %"PRIu64"\n", stats.imissed);
    printf("  RX errors  : %"PRIu64"\n", stats.ierrors);
    printf("  TX errors  : %"PRIu64"\n", stats.oerrors);
}
