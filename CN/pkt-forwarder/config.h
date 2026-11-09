#ifndef CONFIG_H
#define CONFIG_H

#define PF_PORT_ID        0
#define PF_NUM_RX_QUEUES  1
#define PF_NUM_TX_QUEUES  1
#define PF_RX_RING_SIZE   1024
#define PF_TX_RING_SIZE   1024

#define PF_MBUF_POOL_NAME "PF_MBUF_POOL"
#define PF_NUM_MBUFS      8191
#define PF_MBUF_CACHE     250
#define PF_MBUF_PRIV_SZ   0

#define PF_BURST_SIZE     32

#define PF_LCORE_RX       1
#define PF_LCORE_CLS      2   /* classifier */
#define PF_LCORE_QOS      3   /* QoS scheduler + TX */

#endif
