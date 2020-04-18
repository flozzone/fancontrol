//
// Created by floz on 24/09/2019.
//

#include "fan.h"
#include "tim.h"


void fan_init(fan_t *fan) {
    fan->inverted = false;
    fan_set_speed(fan, FAN_RANGE_MIN);
}

void fan_set_speed (const fan_t *fan, speed_t speed) {
    speed_t val = speed;

    if (speed < FAN_RANGE_MIN || speed > FAN_RANGE_MAX) {
        //TODO: handle error
        return;
    }

    if (fan->inverted) {
        val = FAN_RANGE_MAX - speed;
    }

    htim2.Instance->CCR1 = val;
}

speed_t fan_get_speed(const fan_t *fan) {
    speed_t speed = htim2.Instance->CCR1;

    if (fan->inverted) {
        speed = FAN_RANGE_MAX - htim2.Instance->CCR1;
    }

    return speed;
}