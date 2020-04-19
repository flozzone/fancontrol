//
// Created by flozzone on 14.09.19.
//

#ifndef FANCONTROL_APP_H
#define FANCONTROL_APP_H

#include <fan/fan.h>
#include <pid/pid.h>

extern fan_t fan;
extern PID_t pid;

void app_init();
void app_run(void);

#endif //FANCONTROL_APP_H
