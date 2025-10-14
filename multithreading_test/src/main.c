#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/console/console.h>

// Sleep settings 
static const int32_t blink_sleep_ms = 500;
static const int32_t print_sleep_ms = 700;

// Stack size settings
#define BLINK_THREAD_STACK_SIZE 512

// define stack area for threads
K_THREAD_STACK_DEFINE(blink_stack, BLINK_THREAD_STACK_SIZE);

// declare thread data structs
static struct k_thread blink_thread;

// get LED struct from device tree
const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(my_led), gpios);

// blink thread entry point
void blink_thread_start(void *arg_1, void *arg_2, void *arg_3)
{
    int ret;
    int state = 0;
    
    while(1){
        // change state of pin
        state = !state;
        ret = gpio_pin_set_dt(&led, state);
        if (ret < 0) {
            printk("Error setting LED state: %d\n", ret);
        }

        k_msleep(blink_sleep_ms);
    }

}

int main(void){
    int ret;
    k_tid_t blink_tid;

    // make sure GPIO was initialized
    if(!gpio_is_ready_dt(&led)) {
        printk("LED GPIO not ready\n");
        return -1;
    }

    // set gpio as output
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
    if (ret < 0) {
        printk("Error configuring LED GPIO: %d\n", ret);
        return -1;
    }

    // start blink thread
    blink_tid = k_thread_create(&blink_thread,          // Thread struct
                                blink_stack,            // Stack
                                K_THREAD_STACK_SIZEOF(blink_stack),
                                blink_thread_start,     // Entry point
                                NULL,                   // arg_1
                                NULL,                   // arg_2
                                NULL,                   // arg_3
                                7,                      // Priority
                                0,                      // Options
                                K_NO_WAIT);             // Delay

    // do forever
    while(1){
        printk("Hello from main thread!\n");
        k_msleep(print_sleep_ms);
    }
    
    return 0;
}