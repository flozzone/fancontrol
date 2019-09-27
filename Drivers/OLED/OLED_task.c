//
// Created by flozzone on 12.09.19.
//

#include "OLED.h"
#include "OLED_task.h"
#include "tim.h"

#include "ds18b20/ds18b20.h"
#include "menu/menu.h"

char disp_cb_buf[OLED_PRINTF_BUF_SIZE];

void TaskOLED(void const * argument) {
    for (;;) {

        if (menu.page_changed) {
            OLEDClear();
            menu.page_changed = false;
        }
        OLEDString(3, 0, menu.pages[menu.cur_page]->title);

        int i = 0;
        while (menu.pages[menu.cur_page]->items[i].type != MENU_TYPE_NONE) {
            char curr = ' ';
            if (i == menu.cur_item) {
                if (!menu.is_editing)
                    curr = '>';
                else
                    curr = '#';
            }
            menu_item_t *item = &menu.pages[menu.cur_page]->items[i];
            if (!item->display_cb) {
                switch (item->type) {
                    case MENU_TYPE_UINT:
                    OLEDPrintf(MENU_X_ITEM_OFFSET, i * 2 + 3, "%c%s: %u\r", curr, item->label, *item->data_uint);
                        break;
                    case MENU_TYPE_INT:
                    OLEDPrintf(MENU_X_ITEM_OFFSET, i * 2 + 3, "%c%s: %i\r", curr, item->label, *item->data_int);
                        break;
                    case MENU_TYPE_FLOAT: {
                        int val = (*item->data_float) * 10;
                        OLEDPrintf(MENU_X_ITEM_OFFSET, i * 2 + 3, "%c%s: %i.%i\r", curr, item->label,
                                   val / 10, val % 10);
                        break;
                    }
                    case MENU_TYPE_DOUBLE:
                    OLEDPrintf(MENU_X_ITEM_OFFSET, i * 2 + 3, "%c%s: %f\r", curr, item->label, *item->data_double);
                        break;
                    case MENU_TYPE_ENUM:
                    OLEDPrintf(MENU_X_ITEM_OFFSET, i * 2 + 3, "%c%s: %s\r", curr, item->label,
                               item->choices[*item->data_uint]);
                        break;
                    case MENU_TYPE_BOOL:
                    OLEDPrintf(MENU_X_ITEM_OFFSET, i * 2 + 3, "%c%s: %s\r", curr, item->label,
                               (*item->data_bool) ? "ON" : "OFF");
                }
            } else {
                item->display_cb(item, disp_cb_buf, OLED_PRINTF_BUF_SIZE);
                OLEDPrintf(MENU_X_ITEM_OFFSET, i * 2 + 3, "%c%s: %s\r", curr, item->label, disp_cb_buf);
            }
            i++;
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
        osDelay(250);
    }
}