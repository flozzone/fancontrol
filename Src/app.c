//
// Created by flozzone on 14.09.19.
//

#include "app.h"

#include "OLED/OLED.h"
#include "menu/menu.h"

#include "ds18b20/ds18b20.h"
#include "dht22/dht22.h"

#include "PID/pid.h"
#include "Fan/fan.h"

#include "tim.h"

#define MENU_PAGES 3

static bool is_sleeping;
static int long_pressed_cnt;
static osTimerId sleepTimerHandle;

enum mode_choices_e {
    MODE_AUTO,
    MODE_MANUAL
};
char *mode_choices[8] = {"AUTO", "MANUAL"};

PID_t pid;

void sleeptimerCallback(void const * argument);

void setMode(uint8_t mode);
int fan_inc_cb(menu_item_t *item, int multiplier);
int fan_dec_cb(menu_item_t *item, int multiplier);
int fan_disp_cb(menu_item_t *item, char *buffer, int n);
int mode_inc_cb(menu_item_t *item, int multiplier);
int mode_dec_cb(menu_item_t *item, int multiplier);

enum pages_e {
    PAGE1,
    PAGE2,
    PAGE3
};

enum page1_enum {
    PAGE1_MODE,
    PAGE1_TIST,
    PAGE1_TSOLL,
    PAGE1_FAN
};

menu_page_t page1 = {
        .title = "Fan-Control16",
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
                        .type = MENU_TYPE_UINT,
                        .editable = true,
                        .min = 0,
                        .max = 10000,
                        .inc_cb = fan_inc_cb,
                        .dec_cb = fan_dec_cb,
                        .display_cb = fan_disp_cb
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
                        .min = INT32_MIN,
                        .max = INT32_MAX
                },
                {
                        .label = "I",
                        .type = MENU_TYPE_INT,
                        .editable = true,
                        .min = INT32_MIN,
                        .max = INT32_MAX
                },
                {
                        .label = "D",
                        .type = MENU_TYPE_INT,
                        .editable = true,
                        .min = INT32_MIN,
                        .max = INT32_MAX
                },
                {
                        .label = "MIN",
                        .type = MENU_TYPE_INT,
                        .editable = true,
                        .min = 0,
                        .max = INT32_MAX
                },
                {
                        .label = "MAX",
                        .type = MENU_TYPE_INT,
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
                        .type = MENU_TYPE_INT,
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
                        .type = MENU_TYPE_UINT,
                        .editable = true,
                        .min = 1,
                        .max = UINT32_MAX
                },
                {
                        .type = MENU_TYPE_NONE
                }
        }
};

menu_page_t *menu_pages[MENU_PAGES] = {
        &page1,
        &page2,
        &page3
};

void app_init() {
    GPIO_InitTypeDef	init;
    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Pull = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_HIGH;
    init.Pin = DC_EN_Pin;
    HAL_GPIO_Init(DC_EN_GPIO_Port,&init);

    /**

    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Pull = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_HIGH;
    init.Pin = DC_INA_Pin;
    HAL_GPIO_Init(DC_INA_GPIO_Port,&init);

    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Pull = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_HIGH;
    init.Pin = DC_INB_Pin;
    HAL_GPIO_Init(DC_INB_GPIO_Port,&init);
**/
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(DC_EN_GPIO_Port, DC_EN_Pin, GPIO_PIN_SET);
    //HAL_GPIO_WritePin(DC_INA_GPIO_Port, DC_INA_Pin, GPIO_PIN_SET);
   // HAL_GPIO_WritePin(DC_INB_GPIO_Port, DC_INB_Pin, GPIO_PIN_RESET);


    //u8g2_DrawCircle(&u8g2, 64, 40, 10, U8G2_DRAW_ALL);

    OLEDInit();
    Ds18b20_Init(osPriorityNormal);
    DHT22_Init();
    PID_Init(&pid);
    pid.setPoint = 24;
    pid.out_min = 2050;
    pid.out_max = 10000;
    pid.Kp = 50;
    pid.Ki = 8;
    pid.Kd = -4;
    pid.dt = 2;
    OLED_autoSleepEnabled = true;
    fan_init(&pid.out_min, &pid.out_min);

    menu_pages[PAGE1]->items[PAGE1_MODE].data_uint = (uint32_t *) &pid.mode;
    menu_pages[PAGE1]->items[PAGE1_TIST].data_float = &ds18b20[0].Temperature;
    menu_pages[PAGE1]->items[PAGE1_TSOLL].data_float = &pid.setPoint;
    menu_pages[PAGE1]->items[PAGE1_FAN].data_uint = (uint32_t *) &htim2.Instance->CCR1;
    menu_pages[PAGE2]->items[0].data_int = &pid.Kp;
    menu_pages[PAGE2]->items[1].data_int = &pid.Ki;
    menu_pages[PAGE2]->items[2].data_int = &pid.Kd;
    menu_pages[PAGE2]->items[3].data_int = &pid.out_min;
    menu_pages[PAGE2]->items[4].data_int = &pid.out_max;
    menu_pages[PAGE3]->items[0].data_int = &pid.dt;
    menu_pages[PAGE3]->items[1].data_bool = &OLED_autoSleepEnabled;
    menu_pages[PAGE3]->items[2].data_uint = &OLED_autoSleepAfterSec;
    menu_init(&menu, &menu_pages[0], MENU_PAGES);

    /* definition and creation of sleepTimer */
    osTimerDef(sleepTimer, sleeptimerCallback);
    sleepTimerHandle = osTimerCreate(osTimer(sleepTimer), osTimerOnce, NULL);

    setMode(MODE_AUTO);
}

void app_run(void) {
    is_sleeping = false;

    if (xTimerChangePeriod(sleepTimerHandle, OLED_autoSleepAfterSec, 10) != pdPASS )
    {
        /* The reset command was not executed successfully.  Take appropriate
        action here. */
    }
    xTimerStart(sleepTimerHandle, 10);

    for(;;)
    {
        bool key_pressed = ((GPIOC->IDR & (BUTTON_BACK_Pin | BUTTON_OK_Pin)) || (GPIOB->IDR & (BUTTON_UP_Pin | BUTTON_DOWN_Pin | BUTTON_LEFT_Pin | BUTTON_RIGHT_Pin))) ? true : false;
        if ( key_pressed && !is_sleeping ) {
            if (HAL_GPIO_ReadPin(BUTTON_OK_GPIO_Port, BUTTON_OK_Pin) == GPIO_PIN_SET) {
                if (menu_current_item(&menu)->editable) {
                    menu.is_editing = true;
                }
            }
            if (HAL_GPIO_ReadPin(BUTTON_BACK_GPIO_Port, BUTTON_BACK_Pin) == GPIO_PIN_SET) {
                menu.is_editing = false;
            }
            if (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_Port, BUTTON_UP_Pin) == GPIO_PIN_SET) {
                if (!menu.is_editing) {
                    menu_item_prev(&menu);
                } else {
                    menu_item_inc(&menu, long_pressed_cnt);
                }
            }
            if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin) == GPIO_PIN_SET) {
                if (!menu.is_editing) {
                    menu_item_next(&menu);
                } else {
                    menu_item_dec(&menu, long_pressed_cnt);
                }
            }
            if (!menu.is_editing) {
                if (HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_Port, BUTTON_LEFT_Pin) == GPIO_PIN_SET) {
                    menu_page_prev(&menu);
                }
                if (HAL_GPIO_ReadPin(BUTTON_RIGHT_GPIO_Port, BUTTON_RIGHT_Pin) == GPIO_PIN_SET) {
                    menu_page_next(&menu);
                }
            }

            long_pressed_cnt++;
            if (long_pressed_cnt > 5)
                long_pressed_cnt = 5;
        }

        if (key_pressed) {
            /* Ensure the LCD back-light is on, then reset the timer that is
             responsible for turning the back-light off after 5 seconds of
             key inactivity.  Wait 10 ticks for the command to be successfully sent
             if it cannot be sent immediately. */
            OLEDPowerSave(false);
            xTimerChangePeriod(sleepTimerHandle, OLED_autoSleepAfterSec * 1000, 10);
            if (xTimerReset(sleepTimerHandle, 10) != pdPASS) {
                /* The reset command was not executed successfully.  Take appropriate
                action here. */
            }
            is_sleeping = false;
        } else {
            long_pressed_cnt = 1;
        }

        if (!key_pressed) {
            vTaskDelay(100);//Analog clock 1s
        } else {
            vTaskDelay(200);//Analog clock 1s
        }
    }
}

/* sleeptimerCallback function */
void sleeptimerCallback(void const * argument)
{
    /* USER CODE BEGIN sleeptimerCallback */
    if (OLED_autoSleepEnabled) {
        OLEDPowerSave(true);
        is_sleeping = true;
        menu.cur_page = 0;
        menu.cur_item = 0;
        menu.is_editing = false;
    }
    /* USER CODE END sleeptimerCallback */
}

int fan_inc_cb(menu_item_t *item, int multiplier) {
    fan_set_speed(fan_get_percent() + multiplier);
}

int fan_dec_cb(menu_item_t *item, int multiplier) {
    fan_set_speed(fan_get_percent() - multiplier);
}

int fan_disp_cb(menu_item_t *item, char *buffer, int n) {
    return snprintf(buffer, n, "%i %%", fan_get_percent());
}

void setMode(uint8_t mode) {
    menu_item_t *item = menu_pages[PAGE1][PAGE1_MODE].items;
    *item->data_uint = mode;

    if (mode == MODE_AUTO) {
        // AUTO
        menu_pages[PAGE1]->items[PAGE1_FAN].editable = false;
        menu_pages[PAGE1]->items[PAGE1_TSOLL].editable = true;
    } else {
        // MANUAL
        menu_pages[PAGE1]->items[PAGE1_FAN].editable = true;
        menu_pages[PAGE1]->items[PAGE1_TSOLL].editable = false;
    }
}

void toogle_mode(menu_item_t *item) {
    setMode((*item->data_uint + 1) % 2);
}

int mode_inc_cb(menu_item_t *item, int multiplier) {
    toogle_mode(item);
}

int mode_dec_cb(menu_item_t *item, int multiplier) {
    toogle_mode(item);
}


