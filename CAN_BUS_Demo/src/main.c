#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>
#include <zephyr/sys/printk.h>

#if DT_HAS_CHOSEN(zephyr_canbus)
#define CAN_NODE DT_CHOSEN(zephyr_canbus)
#else
#define CAN_NODE DT_NODELABEL(can0)
#endif

static const struct device *const can_dev = DEVICE_DT_GET(CAN_NODE);

static void rx_cb(const struct device *dev, struct can_frame *frame, void *user_data)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(user_data);

    bool ext = (frame->flags & CAN_FRAME_IDE) != 0;
    printk("CAN RX: id=0x%08x %s dlc=%d data=",
           frame->id, ext ? "EXT" : "STD", frame->dlc);
    for (int i = 0; i < frame->dlc; i++) {
        printk("%02X", frame->data[i]);
    }
    printk("\n");
}

void main(void)
{
    if (!device_is_ready(can_dev)) {
        printk("CAN device not ready\n");
        return;
    }

    /* Bitrate from overlay is enough; this is optional override: */
    if (can_set_bitrate(can_dev, 500000)) {
        printk("Failed to set bitrate\n");
        return;
    }

    if (can_start(can_dev)) {
        printk("Failed to start CAN\n");
        return;
    }

    /* Accept ALL standard frames */
    struct can_filter flt = {
        .id = 0x000,
        .mask = 0x000,     /* mask 0 -> don’t care bits */
        .flags = 0         /* standard (11-bit) frames */
    };
    int fid = can_add_rx_filter(can_dev, rx_cb, NULL, &flt);
    if (fid < 0) {
        printk("Add STD filter failed: %d\n", fid);
    }

    /* Accept ALL extended frames */
    flt.flags = CAN_FILTER_IDE;   /* match IDE=1 (29-bit) */
    fid = can_add_rx_filter(can_dev, rx_cb, NULL, &flt);
    if (fid < 0) {
        printk("Add EXT filter failed: %d\n", fid);
    }

    printk("CAN up. Listening…\n");

    /* Optional: send a test frame every 1s */
    while (1) {
        struct can_frame tx = {
            .id = 0x123,
            .dlc = 4,
            .flags = 0,               /* standard frame */
            .data = {0xDE, 0xAD, 0xBE, 0xEF},
        };
        int ret = can_send(can_dev, &tx, K_MSEC(100), NULL, NULL);
        if (ret) {
            printk("TX err %d\n", ret);
        }
        k_sleep(K_SECONDS(1));
    }
}
