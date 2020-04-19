//
// Created by flozzone on 17.04.20.
//
#include <stdlib.h>
#include <float.h>

#include "fan/fan.h"
#include "pid/pid.h"
#include "app.h"
#include "user_menu.h"

void set_control_mode(uint8_t mode);
int fan_edit_cb(menu_item_t *item, int16_t incdec);
int fan_disp_cb(menu_item_t *item, char *buffer, int n);
int mode_edit_cb(menu_item_t *item, int16_t incdec);

char *mode_choices[8] = {"AUTO", "MANUAL"};

menu_page_t page1 = {
        .title = "Fan-Control20",
        .items = {
                {
                        .label = "Mode",
                        .type = MENU_TYPE_ENUM,
                        .editable = true,
                        .min_uint = 0,
                        .max_uint = 1,
                        .item_edit_cb = mode_edit_cb,
                        .choices = mode_choices
                },
                {
                        .label = "T ist",
                        .type = MENU_TYPE_FLOAT,
                        .editable = false,
                        .min_float = 0,
                        .max_float = 70
                },
                {
                        .label = "T soll",
                        .type = MENU_TYPE_FLOAT,
                        .editable = true,
                        .min_float = 0,
                        .max_float = 400
                },
                {
                        .label = "Fan [%]",
                        .type = MENU_TYPE_UINT,
                        .editable = true,
                        .min_uint = 0,
                        .max_uint = 100,
                        .item_edit_cb = fan_edit_cb,
                        .item_display_cb = fan_disp_cb
                },
                {
                        .label = "Error",
                        .type = MENU_TYPE_INT,
                        .editable = false,
                        .min_int = INT16_MIN,
                        .max_int = INT16_MAX
                },
                {
                        .type = MENU_TYPE_NONE
                }
        }
};
menu_page_t page2 = {
        .title = "PID-Regler",
        .items = {
                {
                        .label = "P",
                        .type = MENU_TYPE_INT,
                        .editable = true,
                        .min_int = INT16_MIN,
                        .max_int = INT16_MAX
                },
                {
                        .label = "I",
                        .type = MENU_TYPE_INT,
                        .editable = true,
                        .min_int = INT16_MIN,
                        .max_int = INT16_MAX
                },
                {
                        .label = "D",
                        .type = MENU_TYPE_INT,
                        .editable = true,
                        .min_int = INT16_MIN,
                        .max_int = INT16_MAX
                },
                {
                        .label = "MIN",
                        .type = MENU_TYPE_INT,
                        .editable = true,
                        .min_int = 0,
                        .max_int = INT16_MAX
                },
                {
                        .label = "MAX",
                        .type = MENU_TYPE_INT,
                        .editable = true,
                        .min_int = 0,
                        .max_int = INT16_MAX
                },
                {
                        .type = MENU_TYPE_NONE
                }
        }
};

menu_page_t page3 = {
        .title = "Settings",
        .items = {
                {
                        .label = "cycle time",
                        .type = MENU_TYPE_INT,
                        .editable = true,
                        .min_int = 1,
                        .max_int = INT16_MAX
                },
                {
                        .label = "OLED sleep",
                        .type = MENU_TYPE_BOOL,
                        .editable = true,
                        .min_uint = 0,
                        .max_uint = 1,
                },
                {
                        .label = "sleep after",
                        .type = MENU_TYPE_UINT,
                        .editable = true,
                        .min_uint = 1,
                        .max_uint = UINT16_MAX
                },
                {
                        .type = MENU_TYPE_NONE
                }
        }
};

menu_page_t page_pid_stat = {
        .title = "PID stats",
        .items = {
                { .label = "err", .type = MENU_TYPE_LONG,    .editable = false,     .min_long = INT32_MIN,   .max_long = INT32_MAX },
                { .label = "int", .type = MENU_TYPE_INT,    .editable = false,      .min_int = INT16_MIN,   .max_int = INT16_MAX },
                { .label = "out", .type = MENU_TYPE_INT,    .editable = false,      .min_int = INT16_MIN,   .max_int = INT16_MAX },
                { .label = "der", .type = MENU_TYPE_FLOAT,   .editable = false,     .min_float = FLT_MIN,     .max_float = FLT_MAX },
                { .label = "min", .type = MENU_TYPE_INT,    .editable = false,      .min_int = INT16_MIN,   .max_int = INT16_MAX },
                { .label = "max", .type = MENU_TYPE_INT,    .editable = false,      .min_int = INT16_MIN,   .max_int = INT16_MAX },
                { .type = MENU_TYPE_NONE }
        }
};

menu_page_t *menu_pages[] = {
        &page1,
        &page2,
        &page3,
        &page_pid_stat,
        NULL
};


int fan_edit_cb(menu_item_t *item, int16_t incdec) {
    int16_t val = *item->data_uint + incdec;
    percent_t percent;

    if (val < item->min_uint) {
        percent = item->min_uint;
    } else if (val > item->max_uint) {
        percent = item->max_uint;
    } else {
        percent = val;
    }

    fan_set_speed(&fan, ((FAN_RANGE_MAX - FAN_RANGE_MIN)/100 * percent) + FAN_RANGE_MIN);
}

int fan_disp_cb(menu_item_t *item, char *buffer, int n) {
    speed_t speed = fan_get_speed(&fan);
    percent_t percent = (speed - FAN_RANGE_MIN)/((FAN_RANGE_MAX - FAN_RANGE_MIN)/100);
    utoa(percent, buffer, 10);
}

void set_control_mode(uint8_t mode) {
    menu_item_t *item = menu_pages[PAGE1][PAGE1_MODE].items;
    *item->data_uint = mode;

    if (mode == PID_MODE_AUTO) {
        // AUTO
        menu_pages[PAGE1]->items[PAGE1_FAN].editable = false;
        menu_pages[PAGE1]->items[PAGE1_TSOLL].editable = true;
    } else {
        // MANUAL
        menu_pages[PAGE1]->items[PAGE1_FAN].editable = true;
        menu_pages[PAGE1]->items[PAGE1_TSOLL].editable = false;
    }
}

int mode_edit_cb(menu_item_t *item, int16_t incdec) {

    *item->data_uint += incdec;

    int16_t val = *item->data_uint + incdec;

    if (val < item->min_uint) {
        set_control_mode(item->max_uint);
    } else if (val > item->max_uint) {
        set_control_mode(item->min_uint);
    } else {
        set_control_mode(val);
    }
}