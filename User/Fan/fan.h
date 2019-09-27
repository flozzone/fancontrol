//
// Created by floz on 24/09/2019.
//

#ifndef FANCONTROL_FAN_H
#define FANCONTROL_FAN_H

void fan_init();
void fan_set_speed (int speed);
int fan_speed_to_percent(int speed);
int fan_percent_to_speed(int percent);
int fan_get_percent();
void fan_set_percent(int percent);

#endif //FANCONTROL_FAN_H
