#include "blinky_actor.h"

#include <drivers/gpio.h>
#include <kernel.h>

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#if !DT_NODE_HAS_STATUS(LED0_NODE, okay)
#error "Alias led0 is undefined in device tree"
#endif
#if !DT_NODE_HAS_STATUS(LED1_NODE, okay)
#error "Alias led1 is undefined in device tree"
#endif
#if !DT_NODE_HAS_STATUS(LED2_NODE, okay)
#error "Alias led2 is undefined in device tree"
#endif
#if !DT_NODE_HAS_STATUS(LED3_NODE, okay)
#error "Alias led3 is undefined in device tree"
#endif

static void toggle_led0(struct k_timer *timer);
static void toggle_led1(struct k_timer *timer);
static void toggle_led2(struct k_timer *timer);
static void toggle_led3(struct k_timer *timer);

K_MSGQ_DEFINE(blinky_actor_msgq, sizeof(struct blinky_actor_msg), 10, 4);

K_TIMER_DEFINE(led0_timer, toggle_led0, NULL);
K_TIMER_DEFINE(led1_timer, toggle_led1, NULL);
K_TIMER_DEFINE(led2_timer, toggle_led2, NULL);
K_TIMER_DEFINE(led3_timer, toggle_led3, NULL);

static struct k_timer *timers[] = {[LED0] = &led0_timer,
                                   [LED1] = &led1_timer,
                                   [LED2] = &led2_timer,
                                   [LED3] = &led3_timer};

static const struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

#define FOREACH_LED(FN)                                                        \
    FN(led0)                                                                   \
    FN(led1)                                                                   \
    FN(led2)                                                                   \
    FN(led3)

#define CHECK_LED_READY(led)                                                   \
    if (!device_is_ready(led.port)) {                                          \
        printk("ERROR: %s not ready\n", led.port->name);                       \
        return;                                                                \
    }
#define CONFIGURE_LED(led)                                                     \
    err = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);                     \
    if (err < 0) {                                                             \
        printk("ERROR: failed to configure %s\n", led.port->name);             \
        return;                                                                \
    }

static void blinky_main() {
    static struct blinky_actor_msg msg;
    int err;

    FOREACH_LED(CHECK_LED_READY);

    FOREACH_LED(CONFIGURE_LED);

    /* initialize timers so we can see that its working */
    k_timer_start(&led0_timer, K_NO_WAIT, K_MSEC(2000));
    k_timer_start(&led1_timer, K_NO_WAIT, K_MSEC(1000));
    k_timer_start(&led2_timer, K_NO_WAIT, K_MSEC(500));
    k_timer_start(&led3_timer, K_NO_WAIT, K_MSEC(100));

    for (;;) {
        k_msgq_get(&blinky_actor_msgq, &msg, K_FOREVER);
        k_timer_start(timers[msg.led], K_MSEC(msg.blink_period_ms),
                      K_MSEC(msg.blink_period_ms));
    }
}

static void toggle_led0(struct k_timer *timer) { gpio_pin_toggle_dt(&led0); }
static void toggle_led1(struct k_timer *timer) { gpio_pin_toggle_dt(&led1); }
static void toggle_led2(struct k_timer *timer) { gpio_pin_toggle_dt(&led2); }
static void toggle_led3(struct k_timer *timer) { gpio_pin_toggle_dt(&led3); }

K_THREAD_DEFINE(blinky_thread, 1024, blinky_main, NULL, NULL, NULL,
                K_LOWEST_APPLICATION_THREAD_PRIO, 0, 0);