//
// Created by flozzone on 09.09.19.
//

#ifndef FANCONTROL_OLED_H
#define FANCONTROL_OLED_H

#define USE_U8X8

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "oled_driver.h"

void oled_draw(void);
extern bool OLED_autoSleepEnabled;
extern uint32_t OLED_autoSleepAfterSec;

#ifdef USE_U8X8
#define OLEDString(x, y, s) u8x8_DrawString(&u8x8, x, y, s)
#define OLEDChar(x, y, c) u8x8_DrawGlyph(&u8x8, x, y, c)
#define OLEDClear() u8x8_ClearDisplay(&u8x8)
#define OLEDPowerSave(e) u8x8_SetPowerSave(&u8x8, e)
#else
#define OLEDString(x, y, s) u8g2_DrawString(&u8g2, x, y, s)
#define OLEDClear() u8g2_ClearDisplay(&u8g2)
#define OLEDPowerSave(e) u8g2_SetPowerSave(&u8g2, e)
#endif



void OLEDInit(void);


#endif //FANCONTROL_OLED_H
