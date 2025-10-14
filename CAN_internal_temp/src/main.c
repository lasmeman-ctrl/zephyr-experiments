#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/can.h>
#include <zephyr/sys/printk.h>

static double high_temp;
static double low_temp;

/* ----- helpers for temp ----- */
static int read_temperature(const struct device *dev, struct sensor_value *val)
{
    int ret = sensor_sample_fetch_chan(dev, SENSOR_CHAN_AMBIENT_TEMP);
    if (ret < 0) {
        printk("Could not fetch temperature: %d\n", ret);
        return ret;
    }
    ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, val);
    if (ret < 0) {
        printk("Could not get temperature: %d\n", ret);
    }
    return ret;
}

static void temp_alert_handler(const struct device *dev, const struct sensor_trigger *trig)
{
    struct sensor_value value;
    if (read_temperature(dev, &value) < 0) {
        printk("Reading temperature failed\n");
        return;
    }
    double temp = sensor_value_to_double(&value);
    if (temp <= low_temp) {
        printk("Temperature below threshold: %0.1f°C\n", temp);
    } else if (temp >= high_temp) {
        printk("Temperature above threshold: %0.1f°C\n", temp);
    } else {
        printk("Error: temperature alert triggered without valid condition\n");
    }
}

/* ----- CAN receive callback (so you can see PC->MCU frames) ----- */
static void can_rx_cb(const struct device *dev, struct can_frame *frame, void *user_data)
{
    ARG_UNUSED(dev); ARG_UNUSED(user_data);
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
    /* Temp device from your alias ambient-temp0 */
    const struct device *const tdev = DEVICE_DT_GET(DT_ALIAS(ambient_temp0));
    if (!device_is_ready(tdev)) {
        printk("Temp device %s not ready\n", tdev->name);
        return;
    }

    /* CAN device from chosen zephyr,canbus (your overlay points to &flexcan0) */
#if DT_HAS_CHOSEN(zephyr_canbus)
#define CAN_NODE DT_CHOSEN(zephyr_canbus)
#else
#define CAN_NODE DT_NODELABEL(can0)
#endif
    const struct device *const candev = DEVICE_DT_GET(CAN_NODE);
    if (!device_is_ready(candev)) {
        printk("CAN device not ready\n");
        return;
    }

    /* Start CAN (bitrate from DTS: bus-speed = <…>) */
    if (can_start(candev)) {
        printk("Failed to start CAN\n");
        return;
    }

    /* Install accept-all STD + EXT RX filters so you see cansend() from PC */
    struct can_filter flt = { .id = 0x000, .mask = 0x000, .flags = 0 };
    int fid = can_add_rx_filter(candev, can_rx_cb, NULL, &flt);
    if (fid < 0) printk("Add STD filter failed: %d\n", fid);
    flt.flags = CAN_FILTER_IDE; /* match IDE=1 */
    fid = can_add_rx_filter(candev, can_rx_cb, NULL, &flt);
    if (fid < 0) printk("Add EXT filter failed: %d\n", fid);

    printk("Thermometer Example (%s)\n", CONFIG_ARCH);
    printk("Temp dev: %s, CAN up. Publishing every 1s on ID 0x321\n", tdev->name);

    /* First read + set thresholds */
    struct sensor_value sval;
    if (read_temperature(tdev, &sval) != 0) return;
    double temp_c = sensor_value_to_double(&sval);

    const struct sensor_trigger trig = {
        .chan = SENSOR_CHAN_AMBIENT_TEMP,
        .type = SENSOR_TRIG_THRESHOLD,
    };

    low_temp = temp_c + 0.5;
    if (!sensor_value_from_double(&sval, low_temp)) {
        sensor_attr_set(tdev, SENSOR_CHAN_AMBIENT_TEMP, SENSOR_ATTR_LOWER_THRESH, &sval);
    }
    high_temp = temp_c + 1.5;
    if (!sensor_value_from_double(&sval, high_temp)) {
        sensor_attr_set(tdev, SENSOR_CHAN_AMBIENT_TEMP, SENSOR_ATTR_UPPER_THRESH, &sval);
    }
    sensor_trigger_set(tdev, &trig, temp_alert_handler);

    while (1) {
        if (read_temperature(tdev, &sval) != 0) {
            printk("Temp read failed\n");
            break;
        }
        temp_c = sensor_value_to_double(&sval);
        printk("Temperature is %0.2lf°C\n", temp_c);

        /* Pack temp as 0.01°C (centi-degC) into two bytes, little-endian */
        int16_t centi_c = (int16_t)(temp_c * 100.0f);

        struct can_frame tx = {
            .id    = 0x321,
            .flags = 0,   /* standard frame */
            .dlc   = 2,
        };
        tx.data[0] = (uint8_t)(centi_c & 0xFF);
        tx.data[1] = (uint8_t)((centi_c >> 8) & 0xFF);

        int ret = can_send(candev, &tx, K_MSEC(10), NULL, NULL);
        if (ret) {
            printk("CAN TX err %d\n", ret);
        }

        k_sleep(K_SECONDS(1));
    }
}
