//
// Created by flozzone on 17.04.20.
//

#ifndef FANCONTROL_USER_MENU_H
#define FANCONTROL_USER_MENU_H

#include "menu/menu.h"

enum pages_e {
    PAGE1,
    PAGE2,
    PAGE3,
    PAGE_PID_STATS
};

enum page1_enum {
    PAGE1_MODE,
    PAGE1_TIST,
    PAGE1_TSOLL,
    PAGE1_FAN,
    PAGE1_ERROR
};

enum page_pid_stat_e {
    PAGE_PID_PREVERROR = 0,
    PAGE_PID_INTEGRAL,
    PAGE_PID_OUT,
    PAGE_PID_DERIVATIVE,
    PAGE_PID_OUT_MIN,
    PAGE_PID_OUT_MAX,
};

extern menu_page_t *menu_pages[];
void set_control_mode(uint8_t mode);

void menu_display_percent(char *buf, const uint8_t buf_len, percent_t percent);

#endif //FANCONTROL_USER_MENU_H
