/*
 * Educational RTL8168 PCI driver
 *
 * Current stage:
 *   - PCI device matching
 *   - PCI device enablement
 *   - DMA mask setup
 *   - PCI BAR discovery
 *   - Dynamic MMIO BAR selection
 *   - MMIO mapping
 *   - Safe register reads
 *   - MAC address read
 *
 * NOT implemented yet:
 *   - Hardware reset
 *   - Firmware programming
 *   - DMA rings
 *   - RX
 *   - TX
 *   - Interrupts/MSI-X
 *   - NAPI
 *
 * IMPORTANT:
 * Do not bind this driver to the live NIC while r8169 owns it.
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/io.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/dma-mapping.h>

#define DRV_NAME            "my_rtl8168"

#define REALTEK_VENDOR_ID   0x10ec
#define RTL8168_DEVICE_ID   0x8168

/*
 * RTL8168 register offsets.
 *
 * These are used only for register observation in this stage.
 */
#define RTL8168_MAC0        0x00
#define RTL8168_CHIP_CMD    0x37
#define RTL8168_TX_CONFIG    0x40

struct rtl8168_private {
    void __iomem *mmio_addr;

    int mmio_bar;

    u16 chip_version;

    u8 mac_addr[ETH_ALEN];
};

/*
 * Find a PCI BAR that exposes memory-mapped I/O.
 *
 * Your actual device has:
 *
 *   BAR0 -> I/O
 *   BAR2 -> MMIO
 *   BAR4 -> MMIO
 *
 * Therefore we must not assume BAR0 is MMIO.
 */
static int rtl8168_find_mmio_bar(struct pci_dev *pdev)
{
    int bar;

    for (bar = 0; bar < PCI_STD_NUM_BARS; bar++) {
        unsigned long flags;
        resource_size_t len;

        flags = pci_resource_flags(pdev, bar);
        len = pci_resource_len(pdev, bar);

        if (!len)
            continue;

        if (!(flags & IORESOURCE_MEM))
            continue;

        return bar;
    }

    return -ENODEV;
}

/*
 * Print all PCI BARs.
 */
static void rtl8168_dump_bars(struct pci_dev *pdev)
{
    int bar;

    for (bar = 0; bar < PCI_STD_NUM_BARS; bar++) {
        resource_size_t start;
        resource_size_t len;
        unsigned long flags;

        start = pci_resource_start(pdev, bar);
        len = pci_resource_len(pdev, bar);
        flags = pci_resource_flags(pdev, bar);

        if (!len)
            continue;

        dev_info(&pdev->dev,
                 "BAR%d: start=0x%llx len=0x%llx flags=0x%lx type=%s\n",
                 bar,
                 (unsigned long long)start,
                 (unsigned long long)len,
                 flags,
                 (flags & IORESOURCE_MEM) ? "MMIO" : "I/O");
    }
}

/*
 * Read and print the first six bytes of the NIC MAC register.
 */
static void rtl8168_read_mac(struct rtl8168_private *tp)
{
    int i;

    for (i = 0; i < ETH_ALEN; i++)
        tp->mac_addr[i] =
            readb(tp->mmio_addr + RTL8168_MAC0 + i);
}

/*
 * PCI probe.
 */
static int rtl8168_probe(struct pci_dev *pdev,
                         const struct pci_device_id *id)
{
    struct rtl8168_private *tp;
    void __iomem * const *iomap;
    int mmio_bar;
    int err;
    u32 tx_config;
    u8 chip_cmd;

    dev_info(&pdev->dev, "=== RTL8168 educational probe ===\n");

    dev_info(&pdev->dev,
             "PCI address : %s\n",
             pci_name(pdev));

    dev_info(&pdev->dev,
             "Vendor      : 0x%04x\n",
             pdev->vendor);

    dev_info(&pdev->dev,
             "Device      : 0x%04x\n",
             pdev->device);

    dev_info(&pdev->dev,
             "Revision    : 0x%02x\n",
             pdev->revision);

    /*
     * Enable PCI device.
     */
    err = pcim_enable_device(pdev);
    if (err) {
        dev_err(&pdev->dev,
                "unable to enable PCI device: %d\n",
                err);
        return err;
    }

    /*
     * Show the actual PCI BAR layout.
     */
    rtl8168_dump_bars(pdev);

    /*
     * Find an actual MMIO BAR.
     */
    mmio_bar = rtl8168_find_mmio_bar(pdev);
    if (mmio_bar < 0) {
        dev_err(&pdev->dev,
                "no MMIO BAR found\n");
        return mmio_bar;
    }

    dev_info(&pdev->dev,
             "selected MMIO BAR%d\n",
             mmio_bar);

    dev_info(&pdev->dev,
             "MMIO start = 0x%llx\n",
             (unsigned long long)
                 pci_resource_start(pdev, mmio_bar));

    dev_info(&pdev->dev,
             "MMIO length = 0x%llx\n",
             (unsigned long long)
                 pci_resource_len(pdev, mmio_bar));

    /*
     * Configure DMA addressing capability.
     *
     * We are not performing DMA yet.
     */
    err = dma_set_mask_and_coherent(&pdev->dev,
                                    DMA_BIT_MASK(64));

    if (err) {
        dev_info(&pdev->dev,
                 "64-bit DMA unavailable, trying 32-bit DMA\n");

        err = dma_set_mask_and_coherent(&pdev->dev,
                                        DMA_BIT_MASK(32));

        if (err) {
            dev_err(&pdev->dev,
                    "no suitable DMA mask: %d\n",
                    err);
            return err;
        }

        dev_info(&pdev->dev,
                 "using 32-bit DMA\n");
    } else {
        dev_info(&pdev->dev,
                 "using 64-bit DMA\n");
    }

    /*
     * Reserve and map the selected MMIO BAR.
     */
    err = pcim_iomap_regions(pdev,
                             BIT(mmio_bar),
                             DRV_NAME);

    if (err) {
        dev_err(&pdev->dev,
                "unable to reserve/map BAR%d: %d\n",
                mmio_bar,
                err);
        return err;
    }

    iomap = pcim_iomap_table(pdev);

    if (!iomap || !iomap[mmio_bar]) {
        dev_err(&pdev->dev,
                "BAR%d mapping failed\n",
                mmio_bar);
        return -ENOMEM;
    }

    /*
     * Allocate private driver state.
     *
     * We don't register a net_device yet because RX/TX
     * is not implemented.
     */
    tp = devm_kzalloc(&pdev->dev,
                      sizeof(*tp),
                      GFP_KERNEL);

    if (!tp)
        return -ENOMEM;

    tp->mmio_addr = iomap[mmio_bar];
    tp->mmio_bar = mmio_bar;

    pci_set_drvdata(pdev, tp);

    /*
     * Enable PCI bus mastering.
     *
     * Required later for DMA.
     */
    pci_set_master(pdev);

    dev_info(&pdev->dev,
             "BAR%d mapped at %p\n",
             mmio_bar,
             tp->mmio_addr);

    /*
     * ---------------------------------------------------------
     * Basic hardware register observation.
     * ---------------------------------------------------------
     */

    rtl8168_read_mac(tp);

    dev_info(&pdev->dev,
             "MAC address = %pM\n",
             tp->mac_addr);

    /*
     * CHIP_CMD is an 8-bit register.
     */
    chip_cmd = readb(tp->mmio_addr +
                     RTL8168_CHIP_CMD);

    dev_info(&pdev->dev,
             "ChipCmd = 0x%02x\n",
             chip_cmd);

    /*
     * TxConfig is a 32-bit register.
     *
     * We only report the raw value at this stage.
     * We deliberately do not make hardware-version
     * assumptions here.
     */
    tx_config = readl(tp->mmio_addr +
                      RTL8168_TX_CONFIG);

    dev_info(&pdev->dev,
             "TxConfig = 0x%08x\n",
             tx_config);

    /*
     * Don't reset the NIC.
     *
     * Don't load firmware.
     *
     * Don't allocate DMA rings.
     *
     * Don't register a network interface.
     *
     * This stage is strictly PCI/MMIO discovery.
     */

    dev_info(&pdev->dev,
             "RTL8168 PCI/MMIO discovery successful\n");

    return 0;
}

/*
 * PCI remove.
 */
static void rtl8168_remove(struct pci_dev *pdev)
{
    struct rtl8168_private *tp;

    tp = pci_get_drvdata(pdev);

    dev_info(&pdev->dev,
             "=== RTL8168 educational remove ===\n");

    if (tp)
        dev_info(&pdev->dev,
                 "MMIO BAR%d unmapped\n",
                 tp->mmio_bar);

    pci_clear_master(pdev);
    pci_set_drvdata(pdev, NULL);
}

/*
 * PCI device table.
 */
static const struct pci_device_id rtl8168_ids[] = {
    {
        PCI_DEVICE(REALTEK_VENDOR_ID,
                   RTL8168_DEVICE_ID)
    },
    { }
};

MODULE_DEVICE_TABLE(pci, rtl8168_ids);

/*
 * PCI driver structure.
 */
static struct pci_driver rtl8168_driver = {
    .name = DRV_NAME,
    .id_table = rtl8168_ids,
    .probe = rtl8168_probe,
    .remove = rtl8168_remove,
};

module_pci_driver(rtl8168_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pradeep");
MODULE_DESCRIPTION(
    "Educational RTL8168 PCI/MMIO driver"
);