//
// Created by flozzone on 17.04.20.
//
#include <stdlib.h>
#include <float.h>
#include <string.h>

#include "fan/fan.h"
#include "pid/pid.h"
#include "app.h"
#include "user_menu.h"

#define UNUSED(X) (void)X

int fan_edit_cb(menu_item_t *item);
int fan_disp_cb(menu_item_t *item, char *_buf, int _buf_size);
int fan_min_edit_cb(menu_item_t *item);
int fan_min_disp_cb(menu_item_t *item, char *_buf, int _buf_size);
int fan_max_edit_cb(menu_item_t *item);
int fan_max_disp_cb(menu_item_t *item, char *_buf, int _buf_size);
int mode_edit_cb(menu_item_t *item);

char *mode_choices[8] = {"AUTO", "MANUAL"};

menu_page_t page1 = {
        .title = "Fan-Control",
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
                        .label = "Fan",
                        .type = MENU_TYPE_PERCENT,
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
                        .type = MENU_TYPE_PERCENT,
                        .editable = true,
                        .min_int = 0,
                        .max_int = 100,
                        .item_edit_cb = fan_min_edit_cb,
                        .item_display_cb = fan_min_disp_cb
                },
                {
                        .label = "MAX",
                        .type = MENU_TYPE_PERCENT,
                        .editable = true,
                        .min_int = 0,
                        .max_int = 100,
                        .item_edit_cb = fan_max_edit_cb,
                        .item_display_cb = fan_max_disp_cb
                },
                {
                        .label = "INV",
                        .type = MENU_TYPE_BOOL,
                        .editable = true,
                        .min_uint = 0,
                        .max_uint = 1
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

uint8_t menu_fan_speed_as_percent(speed_t speed) {
    return (speed - FAN_RANGE_MIN)/((FAN_RANGE_MAX - FAN_RANGE_MIN)/100);
}

uint16_t menu_fan_percent_as_speed(percent_t percent) {
    return ((FAN_RANGE_MAX - FAN_RANGE_MIN)/100 * percent) + FAN_RANGE_MIN;
}

void menu_display_fan_speed_as_percent(char *_buf, const uint8_t _buf_size, speed_t speed) {
    utoa(menu_fan_speed_as_percent(speed), _buf, 10);
    uint8_t len = strlen(_buf);
    strncpy(&_buf[len], " %", _buf_size - len);
}

int fan_edit_cb(menu_item_t *item) {
    fan_set_speed(&fan, menu_fan_percent_as_speed(*item->data_uint));
}

int fan_disp_cb(menu_item_t *item, char *_buf, int _buf_size) {
    UNUSED(item);
    speed_t speed = fan_get_speed(&fan);
    menu_display_fan_speed_as_percent(_buf, _buf_size, speed);
}

int fan_min_edit_cb(menu_item_t *item) {
    pid.out_min = menu_fan_percent_as_speed(*item->data_uint);
}

int fan_min_disp_cb(menu_item_t *item, char *_buf, int _buf_size) {
    UNUSED(item);
    menu_display_fan_speed_as_percent(_buf, _buf_size, pid.out_min);
}

int fan_max_edit_cb(menu_item_t *item) {
    pid.out_max = menu_fan_percent_as_speed(*item->data_uint);
}

int fan_max_disp_cb(menu_item_t *item, char *_buf, int _buf_size) {
    UNUSED(item);
    menu_display_fan_speed_as_percent(_buf, _buf_size, pid.out_max);
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



int mode_edit_cb(menu_item_t *item) {
    set_control_mode(*item->data_uint);
}