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

uint16_t get_multiplier(uint16_t num) {
    uint16_t ret = 1;
    switch (num) {
        case 0:
        case 1:
        case 2: ret = 1; break;
        case 3: ret = 2; break;
        case 4: ret = 5; break;
        default: {
            ret = pow(10, num - 4);
        }
    }

    return ret;
}

void menu_item_inc(menu_t *menu, uint16_t multiplier) {
    menu_item_t *item = menu_current_item(menu);

    uint16_t _multiplier = get_multiplier(multiplier);

    if (item->editable) {
        switch (item->type) {
            case MENU_TYPE_BOOL:
            case MENU_TYPE_ENUM:
            case MENU_TYPE_UINT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    uint16_t val = (*item->data_uint) + _multiplier;
                    if (val <= item->max && val > *item->data_uint) {
                        *item->data_uint = val;
                    } else {
                        *item->data_uint = item->max;
                    }
                }
                break;
            }
            case MENU_TYPE_ULONG: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    uint32_t val = (*item->data_ulong) + _multiplier;
                    if (val <= item->max && val > *item->data_ulong) {
                        *item->data_ulong = val;
                    } else {
                        *item->data_ulong = item->max;
                    }
                }
                break;
            }
            case MENU_TYPE_INT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    int16_t val = (*item->data_int) + _multiplier;
                    if (val <= item->max && val > *item->data_int) {
                        *item->data_int = val;
                    } else {
                        *item->data_int = item->max;
                    }
                }
                break;
            }
            case MENU_TYPE_LONG: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    int32_t val = (*item->data_long) + _multiplier;
                    if (val <= item->max && val > *item->data_long) {
                        *item->data_long = val;
                    } else {
                        *item->data_long = item->max;
                    }
                }
                break;
            }
            case MENU_TYPE_DOUBLE: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    double val = (*item->data_double) + _multiplier;
                    if (val <= item->max && val > *item->data_double) {
                        *item->data_double = val;
                    } else {
                        *item->data_double = item->max;
                    }
                }
                break;
            }
            case MENU_TYPE_FLOAT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    float val = (*item->data_float) + 0.1F * _multiplier;
                    if (val <= item->max && val > *item->data_float) {
                        *item->data_float = val;
                    } else {
                        *item->data_float = item->max;
                    }
                }
                break;
            }
        }
    }
}

void menu_item_dec(menu_t *menu, uint16_t multiplier) {
    menu_item_t *item = menu_current_item(menu);
    uint16_t _multiplier = get_multiplier(multiplier);

    if (item->editable) {
        switch (item->type) {
            case MENU_TYPE_BOOL:
            case MENU_TYPE_ENUM:
            case MENU_TYPE_UINT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    uint16_t val = (*item->data_uint) - _multiplier;
                    if (val >= item->min && val < *item->data_uint) {
                        *item->data_uint = val;
                    } else {
                        *item->data_uint = item->min;
                    }
                }
                break;
            }
            case MENU_TYPE_ULONG: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    uint32_t val = (*item->data_ulong) - _multiplier;
                    if (val >= item->min && val < *item->data_ulong) {
                        *item->data_ulong = val;
                    } else {
                        *item->data_ulong = item->min;
                    }
                }
                break;
            }
            case MENU_TYPE_INT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    int16_t val = (*item->data_int) - _multiplier;
                    if (val >= item->min && val < *item->data_int) {
                        *item->data_int = val;
                    } else {
                        *item->data_int = item->min;
                    }
                }
                break;
            }
            case MENU_TYPE_LONG: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    int32_t val = (*item->data_long) - _multiplier;
                    if (val >= item->min && val < *item->data_long) {
                        *item->data_long = val;
                    } else {
                        *item->data_long = item->min;
                    }
                }
                break;
            }
            case MENU_TYPE_DOUBLE: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    double val = (*item->data_double) - _multiplier;
                    if (val >= item->min && val < *item->data_double) {
                        *item->data_double = val;
                    } else {
                        *item->data_double = item->min;
                    }
                }
                break;
            }
            case MENU_TYPE_FLOAT: {
                if (item->inc_cb != NULL) {
                    item->inc_cb(item, _multiplier);
                } else {
                    float val = (*item->data_float) - 0.1F * _multiplier;
                    if (val >= item->min && val < *item->data_float) {
                        *item->data_float = val;
                    } else {
                        *item->data_float = item->min;
                    }
                }
                break;
            }
        }
    }
}

