//
// Created by flozzone on 12.09.19.
//

#ifndef FANCONTROL_MENU_H
#define FANCONTROL_MENU_H

#include <stdbool.h>
#include <stdint-gcc.h>

#define MENU_MAX_LABEL 14
#define MENU_MAX_TITLE 16
#define MENU_MAX_ITEMS 6
#define MENU_MAX_CHOICE_BUF_LEN 8
#define MENU_X_ITEM_OFFSET 0

typedef enum menu_item_type {
    MENU_TYPE_NONE = 0,
    MENU_TYPE_INT,
    MENU_TYPE_UINT,
    MENU_TYPE_DOUBLE,
    MENU_TYPE_FLOAT,
    MENU_TYPE_ENUM,
    MENU_TYPE_BOOL,
    MENU_TYPE_LONG,
    MENU_TYPE_ULONG
} menu_item_type_e;

typedef struct menu_item_s menu_item_t;

typedef int (*menu_item_id_cb_t)(menu_item_t *item, int16_t incdec);
typedef int (*menu_item_display_cb_t)(menu_item_t *item, char *buffer, int n);

typedef struct menu_item_s {
    const char label[MENU_MAX_LABEL];
    const menu_item_type_e type;
    bool editable;
    union {
        int16_t min_int;
        uint16_t min_uint;
        int32_t min_long;
        uint32_t min_ulong;
        float min_float;
        double min_double;
    };
    union {
        int16_t max_int;
        uint16_t max_uint;
        int32_t max_long;
        uint32_t max_ulong;
        float max_float;
        double max_double;
    };
    const menu_item_id_cb_t item_edit_cb;
    const menu_item_display_cb_t item_display_cb;
    union {
        int16_t *data_int;
        uint16_t *data_uint;
        int32_t *data_long;
        uint32_t *data_ulong;
        double *data_double;
        float *data_float;
        bool *data_bool;
    };
    char **choices;
} menu_item_t;

typedef struct menu_page_s {
    char title[MENU_MAX_TITLE];
    uint8_t max_label_length;
    menu_item_t items[];
} menu_page_t;

typedef struct menu_s {
    menu_page_t **pages;
    uint8_t num_pages;
    uint8_t cur_page;
    uint8_t cur_item;
    uint8_t start_item;
    bool page_changed;
    bool is_editing;
} menu_t;

extern menu_t menu;

void menu_init(menu_t *menu, menu_page_t *pages[]);
void menu_page_next(menu_t *menu);
void menu_page_prev(menu_t *menu);
void menu_item_next(menu_t *menu);
void menu_item_prev(menu_t *menu);
void menu_item_edit(menu_t *menu, int16_t incdec);
menu_page_t *menu_cur_page(menu_t *menu);
menu_item_t *menu_current_item(menu_t *menu);


#endif //FANCONTROL_MENU_H
