//
// Created by flozzone on 10.09.19.
//

#include "pid.h"
#include "ds18b20/ds18b20.h"
#include "tim.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
#include <Fan/fan.h>
#include "app.h"

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
    int64_t error;
    int64_t new_integral;
    //float derivative;

    HAL_UART_Transmit(&huart1, testdata, strlen(testdata), 1000);

    in = (int) (_in * 10);
    setPoint = (int) (_setPoint * 10);

    if (!pid->inverted) {
        error = in - setPoint;
    } else {
        error = setPoint - in;
    }

    new_integral = pid->integral + error * pid->dt;
    pid->derivative = (error - pid->prevError)/pid->dt;
    
    *out = (pid->Kp * error) + (pid->Ki * new_integral) + (pid->Kd * pid->derivative);

    pid->out = *out;

    pid->prevError = error;

    if (*out < pid->out_min) {
        *out = pid->out_min;
    } else if (*out > pid->out_max) {
        *out = pid->out_max;
    } else {
        pid->integral = new_integral;
    }
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
        //HAL_StatusTypeDef ret = HAL_UART_Transmit(&huart2, (char *) &data, sizeof(data_serial_t), 500);

        //printf("TESTasdasdasd");

        osDelay(pid->dt * 1000);
    }

}