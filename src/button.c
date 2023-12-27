#include "button.h"

#include <zephyr/drivers/gpio.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include "led.h"

struct gpio_dt_spec buttons[] = {
	[BUTTON_0] = GPIO_DT_SPEC_GET(DT_NODELABEL(button0), gpios),
	[BUTTON_1] = GPIO_DT_SPEC_GET(DT_NODELABEL(button1), gpios),
	[BUTTON_2] = GPIO_DT_SPEC_GET(DT_NODELABEL(button2), gpios),
	[BUTTON_3] = GPIO_DT_SPEC_GET(DT_NODELABEL(button3), gpios),
};

struct gpio_callback button_callbacks[BUTTON_NUM_BUTTONS] = {0};

enum led_index button_to_led_mapping[] = {
	[BUTTON_0] = LED_0,
	[BUTTON_1] = LED_1,
	[BUTTON_2] = LED_2,
	[BUTTON_3] = LED_3,
};

static enum led_index button_pin_to_led_index(uint32_t pin);
static void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pin);

static int button_init(void)
{
	while (!device_is_ready(buttons[BUTTON_0].port) || !device_is_ready(buttons[BUTTON_1].port)
		   || !device_is_ready(buttons[BUTTON_2].port) || !device_is_ready(buttons[BUTTON_3].port))
	{
		k_sleep(K_MSEC(100));
	}

	for (int i = BUTTON_0; i < BUTTON_NUM_BUTTONS; i++)
	{
		struct gpio_dt_spec *button = &buttons[i];

		gpio_pin_configure_dt(button, GPIO_INPUT);
		gpio_pin_interrupt_configure_dt(button, GPIO_INT_EDGE_TO_ACTIVE);
		gpio_init_callback(&button_callbacks[i], button_isr, BIT(button->pin));
		gpio_add_callback(button->port, &button_callbacks[i]);
	}

	return 0;
}

static enum led_index button_pin_to_led_index(uint32_t pin)
{
	for (int i = BUTTON_0; i < BUTTON_NUM_BUTTONS; i++)
	{
		if (pin == BIT(buttons[i].pin))
		{
			return button_to_led_mapping[i];
		}
	}

	CODE_UNREACHABLE;
}

static void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pin)
{
	led_toggle(button_pin_to_led_index(pin));
}

SYS_INIT(button_init, APPLICATION, 0);
