#ifndef __POWER_H__
#define __POWER_H__

#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void power_init(void);

void power_on(uint8_t port);
void power_off(uint8_t port);

void power_cycle(
    uint8_t port,
    uint32_t off_time_ms
);

bool power_is_on(uint8_t port);

#endif // !__POWER_H__
