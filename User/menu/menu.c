//
// Created by flozzone on 12.09.19.
//

#include "menu.h"
#include <string.h>
#include <math.h>

menu_t menu;

void menu_init(menu_t *menu, menu_page_t *pages[]) {
    menu->cur_item = 0;
    menu->cur_page = 0;
    menu->page_changed = false;
    menu->is_editing = false;
    menu->pages = pages;
    menu->start_item = 0;
    menu->num_pages = 0;
    uint8_t len;

    unsigned int i;
    for (i = 0; pages[i] != NULL; i++) {

        menu_item_t *item = pages[i]->items;
        pages[i]->max_label_length = 0;
        while (item->type != MENU_TYPE_NONE) {
             len = strlen(item->label);
            if (len > pages[i]->max_label_length)
                pages[i]->max_label_length = len;
            item++;
        }
    }

    menu->num_pages = i;
}

void menu_page_next(menu_t *menu) {
    if (menu->cur_page < menu->num_pages - 1) {
        menu->cur_page++;
        menu->page_changed = true;
        menu->cur_item = 0;
        menu->start_item = 0;
    }
}

void menu_page_prev(menu_t *menu) {
    if (menu->cur_page > 0) {
        menu->cur_page--;
        menu->page_changed = true;
        menu->cur_item = 0;
        menu->start_item = 0;
    }
}

void menu_item_next(menu_t *menu) {
    if (menu->pages[menu->cur_page]->items[menu->cur_item+1].type != MENU_TYPE_NONE) {
        menu->cur_item++;

        if (menu->cur_item >= MENU_MAX_ITEMS) {
            menu->start_item++;
        }
    }
}

void menu_item_prev(menu_t *menu) {
    if (menu->cur_item > 0) {
        menu->cur_item--;

        if (menu->cur_item < menu->start_item) {
            menu->start_item--;
        }
    }
}

menu_page_t *menu_cur_page(menu_t *menu) {
    return menu->pages[menu->cur_page];
}
menu_item_t *menu_current_item(menu_t *menu) {
    return &menu->pages[menu->cur_page]->items[menu->cur_item];
}

void menu_item_edit(menu_t *menu, int16_t incdec) {
    menu_item_t *item = menu_current_item(menu);

    if (item->editable) {
        if (item->item_edit_cb != NULL) {
            item->item_edit_cb(item, incdec);
        } else {
            switch (item->type) {
                case MENU_TYPE_BOOL:
                case MENU_TYPE_ENUM:
                case MENU_TYPE_UINT: {
                    uint16_t val = (*item->data_uint) + incdec;
                    if (val < item->min_uint) {
                        *item->data_uint = item->min_uint;
                    } else if (val > item->max_uint) {
                        *item->data_uint = item->max_uint;
                    } else {
                        *item->data_uint = val;
                    }
                    break;
                }
                case MENU_TYPE_ULONG: {
                    uint32_t val = (*item->data_ulong) + incdec;
                    if (val < item->min_ulong) {
                        *item->data_ulong = item->min_ulong;
                    } else if (val > item->max_ulong) {
                        *item->data_ulong = item->max_ulong;
                    } else {
                        *item->data_ulong = val;
                    }
                    break;
                }
                case MENU_TYPE_INT: {
                    int16_t val = (*item->data_int) + incdec;
                    if (val < item->min_int) {
                        *item->data_int = item->min_int;
                    } else if (val > item->max_int) {
                        *item->data_int = item->max_int;
                    } else {
                        *item->data_int = val;
                    }
                    break;
                }
                case MENU_TYPE_LONG: {
                    int32_t val = (*item->data_long) + incdec;
                    if (val < item->min_long) {
                        *item->data_long = item->min_long;
                    } else if (val > item->max_long) {
                        *item->data_long = item->max_long;
                    } else {
                        *item->data_long = val;
                    }
                    break;
                }
                case MENU_TYPE_DOUBLE: {
                    double val = (*item->data_double) + incdec;
                    if (val < item->min_double) {
                        *item->data_double = item->min_double;
                    } else if (val > item->max_double) {
                        *item->data_double = item->max_double;
                    } else {
                        *item->data_double = val;
                    }
                    break;
                }
                case MENU_TYPE_FLOAT: {
                    float val = (*item->data_float) + 0.1F * incdec;
                    if (val < item->min_float) {
                        *item->data_float = item->min_float;
                    } else if (val > item->max_float) {
                        *item->data_float = item->max_float;
                    } else {
                        *item->data_float = val;
                    }
                    break;
                }
                default: {
                    //TODO: handle error
                }
            }
        }
    }
}
