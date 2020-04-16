//
// Created by flozzone on 12.09.19.
//

#include "OLED.h"
#include "OLED_task.h"
#include "tim.h"

#include <string.h>

#include "ds18b20/ds18b20.h"
#include "menu/menu.h"
#include "ltoa.h"

#define OLED_MAX_CHARS_ON_LINE 16
#define OLED_BUF_SIZE OLED_MAX_CHARS_ON_LINE

static char buf[OLED_BUF_SIZE];


char *draw_value (menu_item_t *item, char *buf) {
    char *val_ptr = buf;
    if (item->display_cb) {
        item->display_cb(item, buf, OLED_BUF_SIZE);
    } else {
        switch (item->type) {
            case MENU_TYPE_INT:
                itoa(*item->data_int, buf, 10);
                break;
            case MENU_TYPE_UINT:
                utoa(*item->data_uint, buf, 10);
                break;
            case MENU_TYPE_LONG:
                _ltoa(*item->data_long, buf, 10);
                break;
            case MENU_TYPE_ULONG:
                _ultoa(*item->data_ulong, buf, 10);
                break;
            case MENU_TYPE_FLOAT: {
                int val = (*item->data_float) * 10;
                itoa(val / 10, buf, 10);
                uint8_t len = strlen(buf);
                buf[len] = '.';
                buf[len + 1] = 0x30 + (val % 10);
                memset(&buf[len + 2], ' ', OLED_BUF_SIZE - 3 - len);
                buf[OLED_BUF_SIZE - 1] = '\0';
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

    return val_ptr;
}

void oled_draw(void) {
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

        char *val_ptr = draw_value(item, buf);
        uint8_t val_x_offset = 2 + menu.pages[menu.cur_page]->max_label_length;
        uint8_t allowed_val_len = OLED_MAX_CHARS_ON_LINE - val_x_offset;
        size_t val_len = strlen(val_ptr);
        if (val_len > allowed_val_len) {
            val_ptr[allowed_val_len - 1] = '\0';
        }

        OLEDString(val_x_offset, y_coord, val_ptr);

        i++;
        item_nr++;
        item = &menu.pages[menu.cur_page]->items[item_nr];
    }
}

void TaskOLED(void const * argument) {
    for (;;) {
        oled_draw();
        osDelay(50);
    }
}