//
// Created by flozzone on 14.09.19.
//

#include <math.h>
#include <stdint.h>

#include "app.h"

#include "oled/oled.h"
#include "menu/menu.h"

#include "ds18b20/ds18b20.h"
#include "dht22/dht22.h"

#include "pid/pid.h"
#include "fan/fan.h"
#include "tim.h"

#include "user_menu.h"

#define LONG_PRESS_COUNT_LIMIT 7 /* This is limited by the get_multiplicater function */
#define DEBOUNCE_DELAY 120       /* ms to wait to check the keypad again */
#define MULTIPLICATOR_INCREMENT_COUNT 7 /* Waits 7 * DEBOUNCE_DELAY ms for the next multiplicator increment */

enum error_e {
    NO_ERROR = 0,
    ERR_OLED_SLEEP_TIMER_RESET_FAILED
};
long error_nr;
static bool is_sleeping;
static uint16_t long_pressed_cnt;
static uint16_t cnt = 0;
static osTimerId sleepTimerHandle;



PID_t pid;
fan_t fan;
static percent_t fan_speed_percent;
static percent_t fan_min_speed_percent;
static percent_t fan_max_speed_percent;

void sleeptimerCallback(void const * argument);

void app_init() {

    /*
     * Enable additional exceptions, so they don't escalate to
     * HardFault immediately.
     */
    SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk |
                  SCB_SHCSR_BUSFAULTENA_Msk |
                  SCB_SHCSR_MEMFAULTENA_Msk;

    GPIO_InitTypeDef	init;
    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Pull = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

    OLEDInit();
    Ds18b20_Init(osPriorityNormal);
    //DHT22_Init();
    PID_Init(&pid);
    fan_init(&fan);

    pid.setPoint = 30.0F;
    fan_max_speed_percent = 70;
    fan_min_speed_percent = 30;
    fan.inverted = true;

    pid.Kp = 100;
    pid.Ki = 8;
    pid.Kd = 1;
    pid.dt = 2;

    OLED_autoSleepEnabled = true;
    OLED_autoSleepAfterSec = 30;

    // page 1 - Fan control
    menu_pages[PAGE1]->items[PAGE1_MODE].data_uint = &pid.mode;
    menu_pages[PAGE1]->items[PAGE1_TIST].data_float = &ds18b20[0].Temperature;
    menu_pages[PAGE1]->items[PAGE1_TSOLL].data_float = &pid.setPoint;
    menu_pages[PAGE1]->items[PAGE1_FAN].data_uint = &fan_speed_percent;
    menu_pages[PAGE1]->items[PAGE1_ERROR].data_long = &error_nr;

    // page 2 - PID settings
    menu_pages[PAGE2]->items[0].data_int = &pid.Kp;
    menu_pages[PAGE2]->items[1].data_int = &pid.Ki;
    menu_pages[PAGE2]->items[2].data_int = &pid.Kd;
    menu_pages[PAGE2]->items[3].data_uint = &fan_min_speed_percent;
    menu_pages[PAGE2]->items[3].item_edit_cb(&menu_pages[PAGE2]->items[3]);
    menu_pages[PAGE2]->items[4].data_uint = &fan_max_speed_percent;
    menu_pages[PAGE2]->items[4].item_edit_cb(&menu_pages[PAGE2]->items[4]);
    menu_pages[PAGE2]->items[5].data_bool = &fan.inverted;


    // page 3 - Settings
    menu_pages[PAGE3]->items[0].data_int = &pid.dt;
    menu_pages[PAGE3]->items[1].data_bool = &OLED_autoSleepEnabled;
    menu_pages[PAGE3]->items[2].data_ulong = &OLED_autoSleepAfterSec;

    // page 4 - PID stats
    menu_pages[PAGE_PID_STATS]->items[PAGE_PID_INTEGRAL].data_int = &pid.integral;
    menu_pages[PAGE_PID_STATS]->items[PAGE_PID_PREVERROR].data_long = &pid.prevError;
    menu_pages[PAGE_PID_STATS]->items[PAGE_PID_OUT].data_int = &pid.debug_out;
    menu_pages[PAGE_PID_STATS]->items[PAGE_PID_DERIVATIVE].data_float = &pid.debug_derivative;
    menu_pages[PAGE_PID_STATS]->items[PAGE_PID_OUT_MAX].data_int = &pid.out_max;
    menu_pages[PAGE_PID_STATS]->items[PAGE_PID_OUT_MIN].data_int = &pid.out_min;


    menu_init(&menu, menu_pages);

    /* definition and creation of sleepTimer */
    osTimerDef(sleepTimer, sleeptimerCallback);
    sleepTimerHandle = osTimerCreate(osTimer(sleepTimer), osTimerOnce, NULL);

    set_control_mode(PID_MODE_AUTO);
}

uint16_t get_increment(uint16_t num) {
    uint16_t ret = 1;
    switch (num) {
        case 0:
        case 1:
        case 2: ret = 1; break;
        case 3: ret = 2; break;
        case 4: ret = 5; break;
        default: {
            ret = 1 << num - 4;
        }
    }

    return ret;
}

bool read_keypad(void) {
    bool key_pressed = (HAL_GPIO_ReadPin(BUTTON_OK_GPIO_Port, BUTTON_OK_Pin) == GPIO_PIN_SET) || \
                        (HAL_GPIO_ReadPin(BUTTON_BACK_GPIO_Port, BUTTON_BACK_Pin) == GPIO_PIN_SET) || \
                        (HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_Port, BUTTON_LEFT_Pin) == GPIO_PIN_SET) || \
                        (HAL_GPIO_ReadPin(BUTTON_RIGHT_GPIO_Port, BUTTON_RIGHT_Pin) == GPIO_PIN_SET) || \
                        (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_Port, BUTTON_UP_Pin) == GPIO_PIN_SET) || \
                        (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin) == GPIO_PIN_SET);

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
                menu_item_edit(&menu, get_increment(long_pressed_cnt));
            }
        }
        if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin) == GPIO_PIN_SET) {
            if (!menu.is_editing) {
                menu_item_next(&menu);
            } else {
                menu_item_edit(&menu, -get_increment(long_pressed_cnt));
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

        cnt++;
        if (cnt == MULTIPLICATOR_INCREMENT_COUNT) {
            long_pressed_cnt++;
            if (long_pressed_cnt > LONG_PRESS_COUNT_LIMIT)
                long_pressed_cnt = LONG_PRESS_COUNT_LIMIT;
            cnt = 0;
        }
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
            //TODO: process error correctly
            error_nr = ERR_OLED_SLEEP_TIMER_RESET_FAILED;
        }
        is_sleeping = false;
    } else {
        long_pressed_cnt = 1;
    }

    return key_pressed;
}

void app_run(void) {
    is_sleeping = false;

    if (xTimerChangePeriod(sleepTimerHandle, OLED_autoSleepAfterSec * 1000, 10) != pdPASS )
    {
        /* The reset command was not executed successfully.  Take appropriate
        action here. */
    }
    xTimerStart(sleepTimerHandle, 10);

    for(;;)
    {
        bool key_pressed = read_keypad();

        oled_draw ();

        if (!key_pressed) {
            vTaskDelay(50);
        } else {
            vTaskDelay(DEBOUNCE_DELAY);
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




