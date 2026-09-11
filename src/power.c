#include "power.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h" 


void power_init(void){
    gpio_init(RELAY_1_PIN);
    gpio_init(RELAY_2_PIN);
    gpio_init(RELAY_3_PIN);
    gpio_init(RELAY_4_PIN);
    gpio_init(RELAY_5_PIN);
    gpio_init(RELAY_6_PIN);
    gpio_init(RELAY_7_PIN);
    gpio_init(RELAY_8_PIN);

    gpio_set_dir(RELAY_1_PIN, GPIO_OUT);
    gpio_set_dir(RELAY_2_PIN, GPIO_OUT);
    gpio_set_dir(RELAY_3_PIN, GPIO_OUT);
    gpio_set_dir(RELAY_4_PIN, GPIO_OUT);
    gpio_set_dir(RELAY_5_PIN, GPIO_OUT);
    gpio_set_dir(RELAY_6_PIN, GPIO_OUT);
    gpio_set_dir(RELAY_7_PIN, GPIO_OUT);
    gpio_set_dir(RELAY_8_PIN, GPIO_OUT);

    gpio_put(RELAY_1_PIN, 0);
    gpio_put(RELAY_2_PIN, 0);
    gpio_put(RELAY_3_PIN, 0);
    gpio_put(RELAY_4_PIN, 0);
    gpio_put(RELAY_5_PIN, 0);
    gpio_put(RELAY_6_PIN, 0);
    gpio_put(RELAY_7_PIN, 0);
    gpio_put(RELAY_8_PIN, 0);
}

void power_on(uint8_t port){
    switch (port)
    {
    case 1: gpio_put(RELAY_1_PIN, 1); break;
    case 2: gpio_put(RELAY_2_PIN, 1); break;
    case 3: gpio_put(RELAY_3_PIN, 1); break;
    case 4: gpio_put(RELAY_4_PIN, 1); break;
    case 5: gpio_put(RELAY_5_PIN, 1); break;
    case 6: gpio_put(RELAY_6_PIN, 1); break;
    case 7: gpio_put(RELAY_7_PIN, 1); break;
    case 8: gpio_put(RELAY_8_PIN, 1); break;
    default:
        break;
    }
}

void power_off(uint8_t port){
    switch (port)
    {
    case 1: gpio_put(RELAY_1_PIN, 0); break;
    case 2: gpio_put(RELAY_2_PIN, 0); break;
    case 3: gpio_put(RELAY_3_PIN, 0); break;
    case 4: gpio_put(RELAY_4_PIN, 0); break;
    case 5: gpio_put(RELAY_5_PIN, 0); break;
    case 6: gpio_put(RELAY_6_PIN, 0); break;
    case 7: gpio_put(RELAY_7_PIN, 0); break;
    case 8: gpio_put(RELAY_8_PIN, 0); break;
    default:
        break;
    }
}

void power_cycle(
    uint8_t port,
    uint32_t off_time_ms
){
    power_off(port);
    sleep_ms(off_time_ms);
    power_on(port);
}

bool power_is_on(uint8_t port){
    switch (port)
    {
    case 1: return gpio_get(RELAY_1_PIN);
    case 2: return gpio_get(RELAY_2_PIN);
    case 3: return gpio_get(RELAY_3_PIN);
    case 4: return gpio_get(RELAY_4_PIN);
    case 5: return gpio_get(RELAY_5_PIN);
    case 6: return gpio_get(RELAY_6_PIN);
    case 7: return gpio_get(RELAY_7_PIN);
    case 8: return gpio_get(RELAY_8_PIN);
    default:
        return false;
    }
}