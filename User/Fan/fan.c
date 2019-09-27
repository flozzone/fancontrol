//
// Created by floz on 24/09/2019.
//

#include "fan.h"
#include "tim.h"

#define FAN_RANGE_MIN 0
#define FAN_RANGE_MAX 10000
#define FAN_RANGE (FAN_RANGE_MAX - FAN_RANGE_MIN)


static int *fan_min;
static int *fan_max;

void fan_init(int *min, int *max) {
    fan_min = min;
    fan_max = max;
}


int invert(int speed) {
    return 10000 - speed;
}

void fan_set_speed (int speed) {
    int _min, _max;
    _min = *fan_min;
    _max = *fan_max;

    htim2.Instance->CCR1 = invert(speed);
}

int fan_speed_to_percent(int speed) {
    return invert(speed) / 100;
}

int fan_percent_to_speed(int percent) {
    return invert(percent * 100);
}

int fan_get_percent() {
    return fan_speed_to_percent(htim2.Instance->CCR1);
}

void fan_set_percent(int percent) {
    htim2.Instance->CCR1 = fan_percent_to_speed(percent);
}