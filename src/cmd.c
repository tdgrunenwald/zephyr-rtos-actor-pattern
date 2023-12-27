#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>

#include "led.h"

#define RX_TIMEOUT_US 100

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data);
static void handle_char(char cmd);

static uint8_t rx_buf[10] = {0};
static uint8_t tx_buf[] = {"Press 1-4 on the keyboard to toggle LEDs 1-4 on the devkit\n"};
static const struct device *uart = DEVICE_DT_GET(DT_NODELABEL(uart0));

static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
	switch (evt->type)
	{
	case UART_RX_RDY:
		handle_char(evt->data.rx_buf.buf[evt->data.rx.offset]);
		break;

	case UART_RX_DISABLED:
		uart_rx_enable(uart, rx_buf, sizeof(rx_buf), RX_TIMEOUT_US);
		break;

	default:
		break;
	}
}

static void handle_char(char cmd)
{
	switch (cmd)
	{
	case '1':
		led_toggle(LED_0);
		break;
	case '2':
		led_toggle(LED_1);
		break;
	case '3':
		led_toggle(LED_2);
		break;
	case '4':
		led_toggle(LED_3);
		break;
	default:
		break;
	}
}

static int cmd_init(void)
{
	int err;

	while (!device_is_ready(uart))
	{
		k_sleep(K_MSEC(100));
	}

	err = uart_callback_set(uart, uart_cb, NULL);
	if (err)
	{
		return err;
	}

	err = uart_tx(uart, tx_buf, sizeof(tx_buf), SYS_FOREVER_US);
	if (err)
	{
		return err;
	}

	err = uart_rx_enable(uart, rx_buf, sizeof(rx_buf), RX_TIMEOUT_US);
	if (err)
	{
		return err;
	}

	return 0;
}

SYS_INIT(cmd_init, APPLICATION, 0);
