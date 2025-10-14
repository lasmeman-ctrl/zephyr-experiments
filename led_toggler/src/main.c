#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/sys/printk.h>
#include <stdio.h>

/* Our external LED is on Port1, Pin 23 (ARD_D3) */
#define LED_PIN 23

/* Grab the GPIO1 device at compile time from Devicetree */
static const struct device *led_dev = DEVICE_DT_GET(DT_NODELABEL(gpio1));


static const int32_t NXP_sleep_time = 1000; // milliseconds

int main(void)
{
    int state = 0;


    if (!device_is_ready(led_dev)) {
        printk("Error: GPIO1 device not ready\n");
        return 0;
    }

    // Set the GPIO as output
    gpio_pin_configure(led_dev, LED_PIN, GPIO_OUTPUT_INACTIVE | GPIO_ACTIVE_HIGH);

    while(1){
        state = !state;
        printk("LED state: %d\n", state);

        // set pin state
        gpio_pin_set(led_dev, LED_PIN, state);
        k_msleep(NXP_sleep_time);
        
    }
    return 0;
}
