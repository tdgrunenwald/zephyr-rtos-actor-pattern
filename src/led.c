#include "led.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

struct gpio_dt_spec leds[] = {
	[LED_0] = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios),
	[LED_1] = GPIO_DT_SPEC_GET(DT_NODELABEL(led1), gpios),
	[LED_2] = GPIO_DT_SPEC_GET(DT_NODELABEL(led2), gpios),
	[LED_3] = GPIO_DT_SPEC_GET(DT_NODELABEL(led3), gpios),
};

void led_toggle(enum led_index led)
{
	gpio_pin_toggle_dt(&leds[led]);
}

static int led_init(void)
{
	while (!device_is_ready(leds[LED_0].port) || !device_is_ready(leds[LED_1].port)
		   || !device_is_ready(leds[LED_2].port) || !device_is_ready(leds[LED_3].port))
	{
		k_sleep(K_MSEC(100));
	}

	for (int i = LED_0; i < LED_NUM_LEDS; i++)
	{
		gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_ACTIVE);
	}

	return 0;
}

SYS_INIT(led_init, APPLICATION, 1);
