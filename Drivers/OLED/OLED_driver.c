//
// Created by flozzone on 12.09.19.
//

#include "OLED.h"
#include "gpio.h"
#include "spi.h"

#ifdef USE_U8X8
u8x8_t u8x8;
#else
u8g2_t u8g2;
#endif

uint8_t u8x8_byte_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
                               void *arg_ptr)
{
    switch (msg)
    {
        case U8X8_MSG_BYTE_SEND:
            HAL_SPI_Transmit(&hspi1, (uint8_t *) arg_ptr, arg_int, 10000);
            break;
        case U8X8_MSG_BYTE_INIT:
            break;
        case U8X8_MSG_BYTE_SET_DC:
            HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            break;
        default:
            return 0;
    }
    return 1;
}

uint8_t u8x8_stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8,
                                U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int,
                                U8X8_UNUSED void *arg_ptr)
{
    switch (msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:
            HAL_Delay(1);
            break;
        case U8X8_MSG_DELAY_MILLI:
            HAL_Delay(arg_int);
            break;
        case U8X8_MSG_GPIO_DC:
            HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
            break;
        case U8X8_MSG_GPIO_RESET:
            HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, arg_int);
            break;
    }
    return 1;
}

void OLEDDriverInit(void) {
#ifdef USE_U8X8
    u8x8_Setup(&u8x8, u8x8_d_ssd1327_ws_128x128, u8x8_cad_001, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);
    u8x8_SetFont(&u8x8, u8x8_font_8x13_1x2_f);
    u8x8_InitDisplay(&u8x8);
    u8x8_SetPowerSave(&u8x8, 0);
    u8x8_ClearDisplay(&u8x8);
#else
    u8g2_Setup_ssd1327_ws_128x128_1(&u8g2, U8G2_R0, u8x8_byte_4wire_hw_spi, u8x8_stm32_gpio_and_delay);
    u8g2_SetFont(&u8g2, u8g2_font_5x8_tr);
    u8g2_SetDrawColor(&u8g2, 0xff);
    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
    u8g2_ClearDisplay(&u8g2);
#endif
}