#ifndef BLINKY_ACTOR_H
#define BLINKY_ACTOR_H

#include <stdint.h>

enum led {
    LED0,
    LED1,
    LED2,
    LED3,
};

struct blinky_actor_msg {
    enum led led;
    int32_t blink_period_ms;
};

extern struct k_msgq blinky_actor_msgq;

#endif /* BLINKY_ACTOR_H */