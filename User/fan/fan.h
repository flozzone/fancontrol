//
// Created by floz on 24/09/2019.
//

#ifndef FANCONTROL_FAN_H
#define FANCONTROL_FAN_H

#include <stdint.h>
#include <stdbool.h>

#define FAN_RANGE_MIN 0
#define FAN_RANGE_MAX 10000
#define FAN_RANGE (FAN_RANGE_MAX - FAN_RANGE_MIN)

typedef int16_t speed_t;

typedef struct {
    bool inverted;
} fan_t;

void fan_init(fan_t *fan);
speed_t fan_get_speed(const fan_t *fan);
void fan_set_speed (const fan_t *fan, speed_t speed);

#endif //FANCONTROL_FAN_H
