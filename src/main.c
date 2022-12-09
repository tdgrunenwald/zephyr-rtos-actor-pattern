#include <drivers/gpio.h>
#include <kernel.h>

#define STACKSIZE 1024
#define PRIORITY K_LOWEST_APPLICATION_THREAD_PRIO

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)

#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Alias led0 is undefined in device tree"
#endif
#if !DT_NODE_HAS_STATUS(LED1_NODE, okay)
#error "Alias led1 is undefined in device tree"
#endif

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

void blink_led(void *spec, void *period) {
    const struct gpio_dt_spec *led = (struct gpio_dt_spec *)spec;
    const int32_t blink_period = (int32_t)period;
    int err;

    if (!device_is_ready(led->port)) {
        printk("ERROR: %s not ready\n", led->port->name);
        return;
    }

    err = gpio_pin_configure_dt(led, GPIO_OUTPUT_ACTIVE);
    if (err < 0) {
        printk("ERROR: failed to configure %s\n", led->port->name);
        return;
    }

    for (;;) {
        err = gpio_pin_toggle_dt(led);
        if (err < 0) {
            return;
        }
        k_msleep(blink_period);
    }
}

K_THREAD_DEFINE(blink_led0_thread, STACKSIZE, blink_led, &led0, 100, NULL,
                PRIORITY, 0, 0);
K_THREAD_DEFINE(blink_led1_thread, STACKSIZE, blink_led, &led1, 1000, NULL,
                PRIORITY, 0, 0);