//
// Created by pm on 4/13/26.
//
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

static struct net_device *my_dev;

/* ---------- TX Function ---------- */
static netdev_tx_t my_xmit(struct sk_buff *skb, struct net_device *dev) {
    printk(KERN_INFO "my_nic: Packet transmitted (%u bytes)\n", skb->len);

    /* Free packet */
    dev_kfree_skb(skb);

    return NETDEV_TX_OK;
}

/* ---------- Open (ifconfig up) ---------- */
static int my_open(struct net_device *dev) {
    printk(KERN_INFO "my_nic: Device opened\n");
    netif_start_queue(dev);
    return 0;
}

/* ---------- Stop (ifconfig down) ---------- */
static int my_stop(struct net_device *dev) {
    printk(KERN_INFO "my_nic: Device closed\n");
    netif_stop_queue(dev);
    return 0;
}

/* ---------- Netdev Ops ---------- */
static const struct net_device_ops my_netdev_ops = {
    .ndo_open = my_open,
    .ndo_stop = my_stop,
    .ndo_start_xmit = my_xmit,
};

/* ---------- Setup ---------- */
static void my_setup(struct net_device *dev) {
    ether_setup(dev);  // Ethernet device

    dev->netdev_ops = &my_netdev_ops;
    dev->flags |= IFF_NOARP;
    dev->features |= NETIF_F_HW_CSUM;
}

/* ---------- Init ---------- */
static int __init my_init(void) {
    printk(KERN_INFO "my_nic: Initializing...\n");

    my_dev = alloc_netdev(0, "mynic%d", NET_NAME_UNKNOWN, my_setup);
    if (!my_dev)
        return -ENOMEM;

    if (register_netdev(my_dev)) {
        free_netdev(my_dev);
        return -ENODEV;
    }

    printk(KERN_INFO "my_nic: Registered\n");
    return 0;
}

/* ---------- Exit ---------- */
static void __exit my_exit(void) {
    unregister_netdev(my_dev);
    free_netdev(my_dev);
    printk(KERN_INFO "my_nic: Removed\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Minimal NIC Driver");