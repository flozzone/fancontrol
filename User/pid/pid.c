//
// Created by flozzone on 10.09.19.
//
#include <stdio.h>
#include <string.h>

#include "pid.h"
#include "ds18b20/ds18b20.h"
#include "tim.h"
#include "usart.h"
#include "fan/fan.h"
#include "app.h"

#define PID_FLOAT_PRECISION 10

char testdata[] = "data\n\r";

void PID_Init(PID_t *pid) {
    pid->integral = 0;
    pid->prevError = 0;
    pid->mode = PID_MODE_AUTO;

    osThreadDef(myTask_PID, TaskPID, osPriorityNormal, 0, 128);
    pid->taskHandle = osThreadCreate(osThread(myTask_PID), pid);
}

int PID_process(PID_t *pid, float _in, float _setPoint, speed_t *out) {
    int32_t in, setPoint;
    int32_t error;
    int32_t new_integral;
    float derivative;
    uint32_t new_out;

    HAL_UART_Transmit(&huart1, testdata, strlen(testdata), 1000);

    in = (int) (_in * PID_FLOAT_PRECISION);
    setPoint = (int) (_setPoint * PID_FLOAT_PRECISION);

    error = in - setPoint;

    new_integral = pid->integral + error * pid->dt;
    derivative = (error - pid->prevError)/pid->dt;

    new_out = (pid->Kp * error) + (pid->Ki * new_integral) + (pid->Kd * derivative);

    pid->prevError = error;

    if (new_out < pid->out_min) {
        *out = pid->out_min;
    } else if (new_out > pid->out_max) {
        *out = pid->out_max;
    } else {
        pid->integral = new_integral;
        *out = new_out;
    }

    pid->debug_out = *out;
    pid->debug_derivative = derivative;
}

typedef struct {
    float temp;
    float setPoint;
    speed_t pwm;
} data_serial_t;

void TaskPID(void const * _pid) {
    PID_t *pid = (PID_t *)_pid;

    for (;;) {
        speed_t pwm;
        if (pid->mode == PID_MODE_AUTO) {
            PID_process(pid, ds18b20[0].Temperature, pid->setPoint, &pwm);
            fan_set_speed(&fan, pwm);
        }

        data_serial_t data;
        data.temp = ds18b20[0].Temperature;
        data.setPoint = pid->setPoint;
        data.pwm = pwm;

        osDelay(pid->dt * 1000);
    }

}