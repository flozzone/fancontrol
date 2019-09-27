//
// Created by flozzone on 09.09.19.
//

#include "input.h"
#include "cmsis_os.h"

osThreadId 	inputThreadHandle;

void TaskInput(void const * argument);

void inputInit(void *param) {
    osThreadDef(myTask_TaskInput, TaskInput, osPriorityNormal, 0, 128);
    inputThreadHandle = osThreadCreate(osThread(myTask_TaskInput), NULL);
}

void TaskInput(void const * argument) {
    //GUI_DisString_EN(0, 72, "fan", &Font12, FONT_BACKGROUND, WHITE);
    //GUI_DisString_EN(40, 72, periodstr, &Font12, FONT_BACKGROUND, WHITE);
}
