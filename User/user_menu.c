//
// Created by flozzone on 17.04.20.
//

#include <Fan/fan.h>
#include <PID/pid.h>
#include <app.h>
#include <stdlib.h>
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
                        .min = 0,
                        .max = 1,
                        .item_edit_cb = mode_edit_cb,
                        .choices = mode_choices
                },
                {
                        .label = "T ist",
                        .type = MENU_TYPE_FLOAT,
                        .editable = false,
                        .min = 0,
                        .max = 70
                },
                {
                        .label = "T soll",
                        .type = MENU_TYPE_FLOAT,
                        .editable = true,
                        .min = 0,
                        .max = 400
                },
                {
                        .label = "Fan",
                        .type = MENU_TYPE_ULONG,
                        .editable = true,
                        .min = 0,
                        .max = 100,
                        .item_edit_cb = fan_edit_cb,
                        .display_cb = fan_disp_cb
                },
                {
                        .label = "Error",
                        .type = MENU_TYPE_LONG,
                        .editable = false,
                        .min = INT16_MIN,
                        .max = INT16_MAX
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
                        .type = MENU_TYPE_LONG,
                        .editable = true,
                        .min = INT32_MIN,
                        .max = INT32_MAX
                },
                {
                        .label = "I",
                        .type = MENU_TYPE_LONG,
                        .editable = true,
                        .min = INT32_MIN,
                        .max = INT32_MAX
                },
                {
                        .label = "D",
                        .type = MENU_TYPE_LONG,
                        .editable = true,
                        .min = INT32_MIN,
                        .max = INT32_MAX
                },
                {
                        .label = "MIN",
                        .type = MENU_TYPE_LONG,
                        .editable = true,
                        .min = 0,
                        .max = INT32_MAX
                },
                {
                        .label = "MAX",
                        .type = MENU_TYPE_LONG,
                        .editable = true,
                        .min = 0,
                        .max = INT32_MAX
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
                        .type = MENU_TYPE_LONG,
                        .editable = true,
                        .min = 1,
                        .max = INT32_MAX
                },
                {
                        .label = "OLED sleep",
                        .type = MENU_TYPE_BOOL,
                        .editable = true,
                        .min = 0,
                        .max = 1,
                },
                {
                        .label = "sleep after",
                        .type = MENU_TYPE_ULONG,
                        .editable = true,
                        .min = 1,
                        .max = UINT32_MAX
                },
                {
                        .type = MENU_TYPE_NONE
                }
        }
};

menu_page_t page_pid_stat = {
        .title = "PID stats",
        .items = {
                { .label = "err", .type = MENU_TYPE_LONG,    .editable = false, },
                { .label = "int", .type = MENU_TYPE_LONG,    .editable = false, },
                { .label = "out", .type = MENU_TYPE_LONG,    .editable = false, },
                { .label = "der", .type = MENU_TYPE_FLOAT,   .editable = false, },
                { .label = "min", .type = MENU_TYPE_LONG,    .editable = false, },
                { .label = "max", .type = MENU_TYPE_LONG,    .editable = false, },
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

    if (val < item->min) {
        percent = item->min;
    } else if (val > item->max) {
        percent = item->max;
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

    if (val < item->min) {
        set_control_mode(item->max);
    } else if (val > item->max) {
        set_control_mode(item->min);
    } else {
        set_control_mode(val);
    }
}