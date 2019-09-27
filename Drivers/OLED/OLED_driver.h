//
// Created by flozzone on 12.09.19.
//

#ifndef FANCONTROL_OLED_DRIVER_H
#define FANCONTROL_OLED_DRIVER_H

#ifdef USE_U8X8
#include "u8g2/u8x8.h"
#else
#include "u8g2/u8g2.h"
#endif

#ifdef USE_U8X8
extern u8x8_t u8x8;
#else
extern u8g2_t u8g2;
#endif


void OLEDDriverInit(void);

#endif //FANCONTROL_OLED_DRIVER_H
