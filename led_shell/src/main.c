#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>

/* Our external LED is on Port1, Pin 23 (ARD_D3) */
#define LED_PIN 23

/* Grab the GPIO1 device at compile time from Devicetree */
static const struct device *led_dev = DEVICE_DT_GET(DT_NODELABEL(gpio1));
static bool led_state;

/* “on” shell command */
static int cmd_on(const struct shell *sh, size_t argc, char **argv) {
    gpio_pin_set(led_dev, LED_PIN, 1);
    shell_print(sh, "LED ON");
    return 0;
}

/* “off” shell command */
static int cmd_off(const struct shell *sh, size_t argc, char **argv) {
    gpio_pin_set(led_dev, LED_PIN, 0);
    shell_print(sh, "LED OFF");
    return 0;
}

/* “toggle” shell command */
static int cmd_toggle(const struct shell *sh, size_t argc, char **argv) {
    led_state = !led_state;
    gpio_pin_set(led_dev, LED_PIN, led_state);
    shell_print(sh, "LED %s", led_state ? "ON" : "OFF");
    return 0;
}

/* Register our three commands */
SHELL_CMD_REGISTER(on,     NULL, "Turn LED on",    cmd_on);
SHELL_CMD_REGISTER(off,    NULL, "Turn LED off",   cmd_off);
SHELL_CMD_REGISTER(toggle, NULL, "Toggle the LED", cmd_toggle);

void main(void)
{
    if (!device_is_ready(led_dev)) {
        printk("Error: GPIO1 device not ready\n");
        return;
    }
    /* Configure pin 23 as an active‑high output, starting LOW */
    gpio_pin_configure(led_dev, LED_PIN,
                       GPIO_OUTPUT_INACTIVE | GPIO_ACTIVE_HIGH);
    printk("LED shell ready: type 'on', 'off', or 'toggle'\n");
}
