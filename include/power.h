#ifndef __POWER_H__
#define __POWER_H__
#include <stdbool.h>
#include "config.h"

void init_power();

void power_on_relay(int relay);

void power_off_relay(int relay);

bool is_relay_on(int relay);



#endif // !__POWER_H__
