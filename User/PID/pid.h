//
// Created by flozzone on 10.09.19.
//

#ifndef FANCONTROL_PID_H
#define FANCONTROL_PID_H

#include "cmsis_os.h"

#define NORMALIZE 100.0F

typedef enum pid_mode_e {
    AUTOMATIC,
    MANUAL
} pid_mode_t;

typedef struct {
    float setPoint;

    int32_t Kp;
    int32_t Ki;
    int32_t Kd;
    int32_t dt;

    int integral;
    int prevError;

    int32_t out_min;
    int32_t out_max;

    pid_mode_t mode;

    osThreadId 	taskHandle;
} PID_t;

void PID_Init(PID_t *pid);
int PID_process(PID_t *pid, float in, float setPoint, int *out);
void TaskPID(void const * pid);

#endif //FANCONTROL_PID_H
