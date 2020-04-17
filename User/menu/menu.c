//
// Created by flozzone on 12.09.19.
//

#include "menu.h"
#include <string.h>

menu_t menu;

void menu_init(menu_t *menu, menu_page_t *pages[]) {
    menu->cur_item = 0;
    menu->cur_page = 0;
    menu->page_changed = false;
    menu->is_editing = false;
    menu->pages = pages;
    menu->start_item = 0;
    uint8_t len;

    for (unsigned int i = 0; pages[i] != NULL; i++) {
        menu_item_t *item = pages[i]->items;
        pages[i]->max_label_length = 0;
        while (item->type != MENU_TYPE_NONE) {
             len = strlen(item->label);
            if (len > pages[i]->max_label_length)
                pages[i]->max_label_length = len;
            item++;
        }
    }
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

void menu_item_inc(menu_t *menu, int multiplier) {
    menu_item_t *item = menu_current_item(menu);

    if (item->editable) {
        switch (item->type) {
            case MENU_TYPE_BOOL:
            case MENU_TYPE_ENUM:
            case MENU_TYPE_UINT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    uint16_t val = (*item->data_uint) + multiplier;
                    if (val <= item->max) {
                        *item->data_uint = val;
                    }
                }
                break;
            }
            case MENU_TYPE_ULONG: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    uint32_t val = (*item->data_ulong) + multiplier;
                    if (val <= item->max) {
                        *item->data_ulong = val;
                    }
                }
                break;
            }
            case MENU_TYPE_INT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    int16_t val = (*item->data_int) + multiplier;
                    if (val <= item->max) {
                        *item->data_int = val;
                    }
                }
                break;
            }
            case MENU_TYPE_LONG: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    int32_t val = (*item->data_long) + multiplier;
                    if (val <= item->max) {
                        *item->data_long = val;
                    }
                }
                break;
            }
            case MENU_TYPE_DOUBLE: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    double val = (*item->data_double) + multiplier;
                    if (val <= item->max) {
                        *item->data_double = val;
                    }
                }
                break;
            }
            case MENU_TYPE_FLOAT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    float val = (*item->data_float) + 0.1F * multiplier;
                    if (val <= item->max) {
                        *item->data_float = val;
                    }
                }
                break;
            }
        }
    }
}

void menu_item_dec(menu_t *menu, int multiplier) {
    menu_item_t *item = menu_current_item(menu);
    if (item->editable) {
        switch (item->type) {
            case MENU_TYPE_BOOL:
            case MENU_TYPE_ENUM:
            case MENU_TYPE_UINT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    uint16_t val = (*item->data_uint) - multiplier;
                    if (val <= item->max) {
                        *item->data_uint = val;
                    }
                }
                break;
            }
            case MENU_TYPE_ULONG: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    uint32_t val = (*item->data_ulong) - multiplier;
                    if (val <= item->max) {
                        *item->data_ulong = val;
                    }
                }
                break;
            }
            case MENU_TYPE_INT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    int16_t val = (*item->data_int) - multiplier;
                    if (val <= item->max) {
                        *item->data_int = val;
                    }
                }
                break;
            }
            case MENU_TYPE_LONG: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    int32_t val = (*item->data_long) - multiplier;
                    if (val <= item->max) {
                        *item->data_long = val;
                    }
                }
                break;
            }
            case MENU_TYPE_DOUBLE: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    double val = (*item->data_double) - multiplier;
                    if (val <= item->max) {
                        *item->data_double = val;
                    }
                }
                break;
            }
            case MENU_TYPE_FLOAT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, multiplier);
                } else {
                    float val = (*item->data_float) - 0.1F * multiplier;
                    if (val <= item->max) {
                        *item->data_float = val;
                    }
                }
                break;
            }
        }
    }
}

