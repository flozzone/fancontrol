//
// Created by flozzone on 12.09.19.
//

#include "OLED.h"
#include "OLED_task.h"
#include "tim.h"

#include <string.h>
#include <math.h>

#include "ds18b20/ds18b20.h"
#include "menu/menu.h"
#include "ltoa.h"


#define OLED_MAX_CHARS_ON_LINE 16
#define OLED_BUF_SIZE OLED_MAX_CHARS_ON_LINE

static char buf[OLED_BUF_SIZE];

void right_justify(char *str, const size_t buf_size) {
    size_t len = strlen(str);

    str[buf_size-1] = '\0';

    for (char *f = &str[len-1], *t = &str[buf_size-2]; \
            f >= str; f--, t--) {
        *t = *f;
    }

    memset (str, ' ', buf_size - len - 1);
}

char *draw_value (menu_item_t *item, char *buf, uint8_t allowed_val_len) {

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
                int val = abs(round((*item->data_float) * 10));
                char *_buf = buf;

                if (*item->data_float < 0) *_buf++ = '-';
                itoa(val / 10, _buf, 10);
                uint8_t len = strlen(_buf);
                _buf[len] = '.';
                _buf[len + 1] = 0x30 + (val % 10);
                //memset(&buf[len + 2], ' ', OLED_BUF_SIZE - 3 - len);
                _buf[len + 2] = '\0';
                break;
            }
            case MENU_TYPE_ENUM: {
                // TODO: remove this workaround: check for choices index and number of items
                if (*item->data_uint < 2) {
                    //val_ptr = item->choices[*item->data_uint];
                    strncpy(buf, item->choices[*item->data_uint], MENU_MAX_CHOICE_BUF_LEN);
                }
                break;
            }
            case MENU_TYPE_BOOL: {
         /*       if (*item->data_bool == false) {
                    strcpy(buf, "OFF");
                } else {
                    strcpy(buf, "ON");
                }*/
                strcpy(buf, "NA");
                break;
            }
            default: {
                strcpy(buf, "NA");
                break;
            }
        }
    }

    right_justify (buf, allowed_val_len);

    return buf;
}

void oled_draw(void) {
    int i;
    int item_nr;
    menu_item_t *item;
    uint8_t val_x_offset = menu.pages[menu.cur_page]->max_label_length + 2;
    uint8_t allowed_val_len = OLED_MAX_CHARS_ON_LINE - val_x_offset;

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

        char *val_ptr = draw_value(item, buf, allowed_val_len);

/*        size_t val_len = strlen(val_ptr);
        if (val_len > allowed_val_len) {
            val_ptr[allowed_val_len - 1] = '\0';
        }*/

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