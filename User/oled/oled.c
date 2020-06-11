//
// Created by flozzone on 09.09.19.
//

#include "oled.h"
#include "cmsis_os.h"

#include <string.h>
#include <math.h>

#include "menu/menu.h"


#define OLED_MAX_CHARS_ON_LINE 16
#define OLED_BUF_SIZE OLED_MAX_CHARS_ON_LINE

#define OLED_SLEEP_AFTER_SEC_DEFAULT 15

static char buf[OLED_BUF_SIZE];
bool OLED_autoSleepEnabled = true;
uint32_t OLED_autoSleepAfterSec = OLED_SLEEP_AFTER_SEC_DEFAULT;

void OLEDInit(void) {
    OLEDDriverInit();
}

void right_justify(char *str, const size_t buf_size) {
    size_t len = strlen(str);

    str[buf_size-1] = '\0';

    for (char *f = &str[len-1], *t = &str[buf_size-2]; \
            f >= str; f--, t--) {
        *t = *f;
    }

    memset (str, ' ', buf_size - len - 1);
}

void oled_draw(void) {
    int i;
    int item_nr;
    menu_item_t *item;
    uint8_t val_x_offset = menu.pages[menu.cur_page]->max_label_length + 2;
    uint8_t allowed_val_len = OLED_MAX_CHARS_ON_LINE - val_x_offset + 1;

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

        char *val_ptr = menu_item_print (item, buf, allowed_val_len);

        right_justify (buf, allowed_val_len);

        OLEDString(val_x_offset, y_coord, val_ptr);

        i++;
        item_nr++;
        item = &menu.pages[menu.cur_page]->items[item_nr];
    }
}
