#include <rte_ring.h>
#include <rte_log.h>
#include "ring.h"

#define RTE_LOGTYPE_RING RTE_LOGTYPE_USER2

struct rte_ring *g_rx_ring        = NULL;
struct rte_ring *g_tc_ring[TC_MAX] = {NULL};

static const char *tc_ring_names[TC_MAX] = {
    RING_TC_HI_NAME,
    RING_TC_MED_NAME,
    RING_TC_LO_NAME,
    RING_TC_BG_NAME,
};

int rings_init(void)
{
    /* SPSC: single producer (RX core), single consumer (classifier) */
    g_rx_ring = rte_ring_create(RING_RX_NAME, RING_SIZE,
                                rte_socket_id(),
                                RING_F_SP_ENQ | RING_F_SC_DEQ);
    if (!g_rx_ring) {
        RTE_LOG(ERR, RING, "Failed to create RX ring\n");
        return -1;
    }
    RTE_LOG(INFO, RING, "RX ring created: %u slots\n", RING_SIZE);

    /* Per-class rings: SPSC (classifier produces, QoS consumes) */
    for (int i = 0; i < TC_MAX; i++) {
        g_tc_ring[i] = rte_ring_create(tc_ring_names[i], RING_SIZE,
                                       rte_socket_id(),
                                       RING_F_SP_ENQ | RING_F_SC_DEQ);
        if (!g_tc_ring[i]) {
            RTE_LOG(ERR, RING, "Failed to create TC ring %d\n", i);
            return -1;
        }
    }
    RTE_LOG(INFO, RING, "TC rings created (HI/MED/LO/BG)\n");
    return 0;
}

void rings_free(void)
{
    rte_ring_free(g_rx_ring);
    for (int i = 0; i < TC_MAX; i++)
        rte_ring_free(g_tc_ring[i]);
}
