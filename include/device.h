#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    char model[16];
    char serial[16];

    uint8_t hardware_major;
    uint8_t hardware_minor;

    uint8_t firmware_major;
    uint8_t firmware_minor;
    uint8_t firmware_patch;

    uint8_t uart_count;
    uint8_t power_count;

    bool current_sensor;
} device_info_t;

#endif // !__DEVICE_H__
