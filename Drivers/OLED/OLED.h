//
// Created by flozzone on 09.09.19.
//

#ifndef FANCONTROL_OLED_H
#define FANCONTROL_OLED_H

#define USE_U8X8

#include "OLED_task.h"
#include "OLED_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define OLED_PRINTF_BUF_SIZE 32

extern char oledBuf[OLED_PRINTF_BUF_SIZE];
extern bool OLED_autoSleepEnabled;
extern uint32_t OLED_autoSleepAfterSec;

#ifdef USE_U8X8
#define OLEDString(x, y, s) u8x8_DrawString(&u8x8, x, y, s)
#define OLEDClear() u8x8_ClearDisplay(&u8x8)
#define OLEDPowerSave(e) u8x8_SetPowerSave(&u8x8, e)
#else
#define OLEDString(x, y, s) u8g2_DrawString(&u8g2, x, y, s)
#define OLEDClear() u8g2_ClearDisplay(&u8g2)
#define OLEDPowerSave(e) u8g2_SetPowerSave(&u8g2, e)
#endif

#define OLEDPrintf(x, y, f, ...) snprintf(oledBuf, OLED_PRINTF_BUF_SIZE, f, __VA_ARGS__); OLEDString(x, y, oledBuf);

void OLEDInit(void);


#endif //FANCONTROL_OLED_H
