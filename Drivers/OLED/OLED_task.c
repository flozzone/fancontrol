//
// Created by flozzone on 12.09.19.
//

#include "OLED.h"
#include "OLED_task.h"
#include "tim.h"

#include <string.h>

#include "ds18b20/ds18b20.h"
#include "menu/menu.h"

#define OLED_BUF_SIZE 16

static char buf[OLED_BUF_SIZE];


void draw_value (uint8_t i, menu_item_t *item, uint8_t val_start_x, uint8_t y_coord) {
    char *val_ptr = buf;

    if (item->display_cb) {
        item->display_cb(item, buf, OLED_BUF_SIZE);
    } else {
        switch (item->type) {
            case MENU_TYPE_INT:
                itoa(*item->data_int, buf, 10);
                break;
            case MENU_TYPE_UINT:
                itoa(*item->data_uint, buf, 10);
                break;
            case MENU_TYPE_LONG:
                itoa(*item->data_long, buf, 10);
                break;
            case MENU_TYPE_ULONG:
                itoa(*item->data_ulong, buf, 10);
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
                strcpy(buf, "NA");
                break;
            }
        }
    }

    OLEDString(val_start_x, y_coord, val_ptr);
}

void TaskOLED(void const * argument) {
    for (;;) {

        int i;
        int item_nr;
        menu_item_t *item;

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
                && (i < MENU_MAX_ITEMS)) {

            uint8_t y_coord = i * 2 + 2;

            char curr = ' ';
            if (item_nr == menu.cur_item) {
                curr = menu.is_editing ? '#' : '>';
            }

            OLEDChar(MENU_X_ITEM_OFFSET, y_coord, curr);
            OLEDString(MENU_X_ITEM_OFFSET + 1, y_coord, item->label);

            draw_value(i, item, 2 + menu.pages[menu.cur_page]->max_label_length, y_coord);

            i++;
            item_nr++;
            item = &menu.pages[menu.cur_page]->items[item_nr];
        }

        osDelay(50);
    }
}