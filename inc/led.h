#ifndef LED_H
#define LED_H

enum led_index {
	LED_0,
	LED_1,
	LED_2,
	LED_3,
	LED_NUM_LEDS,
};

void led_toggle(enum led_index led);

void led_init(void);

#endif /* LED_H */
