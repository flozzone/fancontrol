//
// Created by flozzone on 12.09.19.
//

#include "OLED.h"
#include "OLED_task.h"
#include "tim.h"

#include <string.h>

#include "ds18b20/ds18b20.h"
#include "menu/menu.h"

#define OLED_PRINTF_BUF_SIZE 16


extern char oledBuf[OLED_PRINTF_BUF_SIZE];
char disp_cb_buf[OLED_PRINTF_BUF_SIZE];

//#define OLEDPrintf(x, y, f, ...) snprintf(oledBuf, OLED_PRINTF_BUF_SIZE, f, __VA_ARGS__); OLEDString(x, y, oledBuf);

#define OLEDPrintf(x, y, f, ...) {} while (0);


void TaskOLED(void const * argument) {
    for (;;) {
        char buf[16];
        int i;
        int item_nr;
        menu_item_t *item;
        char *val_ptr;

        if (menu.page_changed) {
            OLEDClear();
            menu.page_changed = false;
        }

        OLEDString(3, 0, menu.pages[menu.cur_page]->title);

        buf[0] = '\0';
        i = 0;
        item_nr = menu.start_item;
        item = &menu.pages[menu.cur_page]->items[item_nr];
        while ((item->type != MENU_TYPE_NONE)
                && (i < menu.start_item + MENU_MAX_ITEMS)) {
            char curr = ' ';
            if (i == menu.cur_item) {
                if (!menu.is_editing)
                    curr = '>';
                else
                    curr = '#';
            }

            val_ptr = buf;
            uint8_t y_label = i * 2 + 3;
            OLEDChar(MENU_X_ITEM_OFFSET, y_label, curr);
            OLEDString(MENU_X_ITEM_OFFSET + 1, y_label, item->label);

            if (!item->display_cb) {

                switch (item->type) {
                    case MENU_TYPE_INT:
                        itoa(*item->data_int, buf, 10);
                        break;
                    case MENU_TYPE_UINT:
                        itoa(*item->data_uint, buf, 10);
                        break;
                    case MENU_TYPE_FLOAT: {
                        int val = (*item->data_float) * 10;
                        itoa(val / 10, buf, 10);
                        uint8_t len = strlen(buf);
                        buf[len] = '.';
                        buf[len + 1] = 0x30 + (val % 10);
                        buf[len + 2] = '\0';
                        break;
                    }
                    case MENU_TYPE_ENUM: {
                        // TODO: remove this workaround: check for choices index and number of items
                        if (*item->data_uint < 2) {
                            val_ptr = item->choices[*item->data_uint];
                        }
                        break;
                    }
                    default: {
                        strcpy(buf, "unsupp");
                        break;
                    }
                }
            } else {
                item->display_cb(item, disp_cb_buf, OLED_PRINTF_BUF_SIZE);
                OLEDPrintf(MENU_X_ITEM_OFFSET, i * 2 + 3, "%c%s: %s", curr, item->label, disp_cb_buf);
            }

            OLEDString(MENU_LINE_XVAL_START, y_label, val_ptr);

            if (i < MENU_MAX_ITEMS-1) {
                i++;
            }
            item_nr++;

            item = &menu.pages[menu.cur_page]->items[item_nr];
        }

        /**
        int tmp;
        tmp = (int) (ds18b20[0].Temperature * 10);
        OLEDPrintf(2, 4, "temp1: %i.%i\r", tmp / 10, tmp % 10)
        tmp = (int) (ds18b20[1].Temperature * 10);
        OLEDPrintf(2, 6, "temp2: %i.%i\r", tmp / 10, tmp % 10)
        OLEDPrintf(2, 8, "pwm: %i\r", htim2.Instance->CCR1);
         **/



        //OLED_ClearWindow(10, 40, 127, 100, WHITE);
        //GUI_DisString_EN(0, 52, "temp", &Font12, FONT_BACKGROUND, WHITE);
        //GUI_DisString_EN(40, 52, tempstr, &Font12, FONT_BACKGROUND, WHITE);
        //OLED_DisWindow(10, 40, 127, 100);
        //OLED_Display();

        //uint32_t ret = DHT22_GetReadings();
        //DHT22_DecodeReadings();
        //uint16_t humid = DHT22_GetHumidity();
        //uint16_t temp = DHT22_GetTemperature();

        //snprintf(str, BUF_LEN, "humid: %i.%i%%\r", humid / 10, humid % 10);
        //u8x8_DrawString(&u8x8, 2, 8, str);
        //snprintf(str, BUF_LEN, "temp3: %i.%i C\r", temp / 10, temp % 10);
        //u8x8_DrawString(&u8x8, 2, 10, str);
        osDelay(50);
    }
}