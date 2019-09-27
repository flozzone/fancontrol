//
// Created by flozzone on 09.09.19.
//

#include "OLED.h"
#include "cmsis_os.h"

osThreadId 	OLEDTaskHandle;
char oledBuf[OLED_PRINTF_BUF_SIZE];
bool OLED_autoSleepEnabled = true;
uint32_t OLED_autoSleepAfterSec = 15;

void OLEDInit(void) {
    OLEDDriverInit();

    osThreadDef(myTask_OLED, TaskOLED, osPriorityNormal, 0, 128);
    OLEDTaskHandle = osThreadCreate(osThread(myTask_OLED), NULL);
}

