#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

static const int32_t sleep_time_ms = 500;
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_ALIAS(my_button), gpios);

int main(void){
    int ret;
    int state;

    // make sure button is ready
    if (!gpio_is_ready_dt(&btn)) {
        printf("Button device not ready\n");
        return 0;
    }

    // set button as input
    ret = gpio_pin_configure_dt(&btn, GPIO_INPUT);
    if (ret < 0) {
        return 0;
    }

    // print out flags
    printk("Button spec flags: 0x%x\n", btn.dt_flags);


    while(1){
        //poll button state
        state = gpio_pin_get_dt(&btn);
        if (state < 0) {
            printk("Error %d: unable to read button pin\n", state);
        } else {
            printk("Button state: %d\n", state);
        }

        k_msleep(sleep_time_ms);
    }
    return 0;
}
