//
// Created by flozzone on 10.09.19.
//

#ifndef FANCONTROL_PID_H
#define FANCONTROL_PID_H

#include "fan/fan.h"
#include "cmsis_os.h"

typedef enum pid_mode_e {
    PID_MODE_AUTO = 0,
    PID_MODE_MANUAL = 1
} pid_mode_t;

typedef struct {
    float setPoint;

    int16_t Kp;
    int16_t Ki;
    int16_t Kd;
    int16_t dt;

    int16_t integral;
    int32_t prevError;

    int16_t out_min;
    int16_t out_max;

    uint16_t mode;

    // debugging
    float debug_derivative;
    speed_t debug_out;

    osThreadId 	taskHandle;
} PID_t;

void PID_Init(PID_t *pid);
int PID_process(PID_t *pid, float in, float setPoint, speed_t *out);
void TaskPID(void const * pid);

#endif //FANCONTROL_PID_H
