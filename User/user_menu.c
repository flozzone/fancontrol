//
// Created by flozzone on 17.04.20.
//

#include <Fan/fan.h>
#include <PID/pid.h>
#include "user_menu.h"

void set_control_mode(uint8_t mode);
int fan_inc_cb(menu_item_t *item, int multiplier);
int fan_dec_cb(menu_item_t *item, int multiplier);
int fan_disp_cb(menu_item_t *item, char *buffer, int n);
int mode_inc_cb(menu_item_t *item, int multiplier);
int mode_dec_cb(menu_item_t *item, int multiplier);

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
                        .inc_cb = mode_inc_cb,
                        .dec_cb = mode_dec_cb,
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
                        .max = 10000,
                        //.inc_cb = fan_inc_cb,
                        //.dec_cb = fan_dec_cb,
                        //.display_cb = fan_disp_cb
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
                { .label = "  e", .type = MENU_TYPE_LONG,    .editable = false, },
                { .label = "  i", .type = MENU_TYPE_LONG,    .editable = false, },
                { .label = "  o", .type = MENU_TYPE_LONG,    .editable = false, },
                { .label = "o_n", .type = MENU_TYPE_LONG,    .editable = false, },
                { .label = "i_n", .type = MENU_TYPE_LONG,    .editable = false, },
                { .label = "  d", .type = MENU_TYPE_FLOAT,   .editable = false, },
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


int fan_inc_cb(menu_item_t *item, int multiplier) {
    fan_set_speed(fan_get_percent() + multiplier);
}

int fan_dec_cb(menu_item_t *item, int multiplier) {
    fan_set_speed(fan_get_percent() - multiplier);
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

void toogle_control_mode(menu_item_t *item) {
    set_control_mode((*item->data_uint + 1) % 2);
}

int mode_inc_cb(menu_item_t *item, int multiplier) {
    toogle_control_mode(item);
}

int mode_dec_cb(menu_item_t *item, int multiplier) {
    toogle_control_mode(item);
}