//
// Created by flozzone on 12.09.19.
//

#include "menu.h"

menu_t menu;

void menu_init(menu_t *menu, menu_page_t *pages[], int num_pages) {
    menu->num_pages = num_pages;
    menu->cur_item = 0;
    menu->cur_page = 0;
    menu->page_changed = false;
    menu->is_editing = false;
    menu->pages = pages;
}

void menu_page_next(menu_t *menu) {
    if (menu->cur_page < menu->num_pages - 1) {
        menu->cur_page++;
        menu->page_changed = true;
        menu->cur_item = 0;
    }
}

void menu_page_prev(menu_t *menu) {
    if (menu->cur_page > 0) {
        menu->cur_page--;
        menu->page_changed = true;
        menu->cur_item = 0;
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
            case MENU_TYPE_UINT:
                if (*item->data_uint < item->max) {
                    if (!item->inc_cb) {
                        (*item->data_uint) += multiplier;
                    } else {
                        item->inc_cb(item, multiplier);
                    }
                }
                break;
            case MENU_TYPE_INT:
                if (*item->data_int < item->max) {
                    if (!item->dec_cb) {
                        (*item->data_int) += multiplier;
                    } else {
                        item->inc_cb(item, multiplier);
                    }
                }
                break;
            case MENU_TYPE_DOUBLE:
                if (*item->data_double < item->max) {
                    if (!item->dec_cb) {
                        (*item->data_double) += multiplier;
                    } else {
                        item->inc_cb(item, multiplier);
                    }
                }
                break;
            case MENU_TYPE_FLOAT:
                if (*item->data_float < item->max) {
                    if (!item->dec_cb) {
                        (*item->data_float) += multiplier * 0.1F;
                    } else {
                        item->inc_cb(item, multiplier);
                    }
                }
                break;
        }
    }
}

void menu_item_dec(menu_t *menu, int multiplier) {
    menu_item_t *item = menu_current_item(menu);
    if (item->editable) {
        switch (item->type) {
            case MENU_TYPE_BOOL:
            case MENU_TYPE_ENUM:
            case MENU_TYPE_UINT:
                if (*item->data_uint > item->min) {
                    if (!item->dec_cb) {
                        (*item->data_uint) -= multiplier;
                    } else {
                        item->dec_cb(item, multiplier);
                    }

                }
                break;
            case MENU_TYPE_INT:
                if (*item->data_int > item->min) {
                    if (!item->dec_cb) {
                        (*item->data_int) -= multiplier;
                    } else {
                        item->dec_cb(item, multiplier);
                    }
                }
                break;
            case MENU_TYPE_DOUBLE:
                if (*item->data_double > item->min) {
                    if (!item->dec_cb) {
                        (*item->data_double) -= multiplier;
                    } else {
                        item->dec_cb(item, multiplier);
                    }
                }
                break;
            case MENU_TYPE_FLOAT:
                if (*item->data_float > item->min) {
                    if (!item->dec_cb) {
                        (*item->data_float) -= multiplier * 0.1F;
                    } else {
                        item->dec_cb(item, multiplier);
                    }
                }
                break;
        }
    }
}