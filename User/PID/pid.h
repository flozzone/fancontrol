//
// Created by flozzone on 10.09.19.
//

#ifndef FANCONTROL_PID_H
#define FANCONTROL_PID_H

#include "cmsis_os.h"

//#define NORMALIZE_I 100
//#define NORMALIZE 100.0F
#define NORMALIZE_I 1
#define NORMALIZE 1

typedef enum pid_mode_e {
    PID_MODE_AUTO = 0,
    PID_MODE_MANUAL = 1
} pid_mode_t;

typedef struct {
    float setPoint;

    int32_t Kp;
    int32_t Ki;
    int32_t Kd;
    int32_t dt;

    int32_t integral;
    int32_t prevError;

    int32_t out_min;
    int32_t out_max;
    uint8_t inverted;

    uint16_t mode;

    // debugging
    int32_t new_integral;
    float derivative;
    int32_t out;
    int32_t out_norm;

    osThreadId 	taskHandle;
} PID_t;

void PID_Init(PID_t *pid);
int PID_process(PID_t *pid, float in, float setPoint, int *out);
void TaskPID(void const * pid);

#endif //FANCONTROL_PID_H
