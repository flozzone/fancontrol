//
// Created by flozzone on 19.04.20.
//

#include <string.h>
#include <app.h>
#include <ds18b20/ds18b20.h>

#include "serial.h"
#include "usart.h"
#include "user_menu.h"

char testdata[] = "data\n\r";

void task_serial(void const * _serial);

static data_packet_t packet = {
        .magic = SERIAL_BEGIN_DATA_MAGIC
};

void serial_init(serial_t *serial) {
    osThreadDef(taskSerial, task_serial, osPriorityNormal, 0, 128);
    serial->serialThread = osThreadCreate(osThread(taskSerial), serial);
}

void fill_packet(data_packet_t *packet) {
    packet->temp = ds18b20[0].Temperature * 10;
    packet->fan = menu_fan_speed_as_percent(fan_get_speed(&fan));
}

void task_serial(void const * _serial) {
    serial_t *serial = (serial_t *) _serial;

    while (1) {


        fill_packet(&packet);

        HAL_UART_Transmit(&huart1, (char *) &packet, sizeof(data_packet_t), 1000);

        osDelay(500);
    }
}