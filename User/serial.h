//
// Created by flozzone on 19.04.20.
//

#ifndef FANCONTROL_SERIAL_H
#define FANCONTROL_SERIAL_H

#include <menu/menu.h>
#include "cmsis_os.h"

#define SERIAL_BEGIN_DATA_MAGIC "\x74\x0f\xaf\x10\x02\x6a\x9d\xe4\x9d\xb7\xd4\xd8\xfe\xde\x03\x17\
                                    0b\xfe\x11\x9f\xa6\x03\xcd\x9e\x1c\x07\x1f\xe7\xc9\x15\x98\xe1"

typedef struct {
    osThreadId serialThread;
} serial_t;

typedef struct {
    uint32_t magic;
    uint16_t temp;
    percent_t fan;
} data_packet_t;

void serial_init(serial_t *serial);



#endif //FANCONTROL_SERIAL_H
