#include "power.h"

#include "hardware/gpio.h"

const int RELAY_PINS[NUM_RELAYS] = {RELAY_1_PIN, RELAY_2_PIN, RELAY_3_PIN,
         RELAY_4_PIN, RELAY_5_PIN, RELAY_6_PIN, RELAY_7_PIN, RELAY_8_PIN};
         
void init_power() {
    for (int i = 0; i < NUM_RELAYS; i++) {
        gpio_init(RELAY_PINS[i]);
        gpio_set_dir(RELAY_PINS[i], GPIO_OUT);
        gpio_put(RELAY_PINS[i], 1); // Inicializa todos os relés desligados
    }
}

void power_on_relay(int relay) {
    if (relay >= 1 && relay <= NUM_RELAYS) {
        gpio_put(RELAY_PINS[relay - 1], 0); // Liga o relé (inverso)
    }
}

void power_off_relay(int relay) {
    if (relay >= 1 && relay <= NUM_RELAYS) {
        gpio_put(RELAY_PINS[relay - 1], 1); // Desliga o relé (inverso)
    }
}

bool is_relay_on(int relay) {
    if (relay >= 1 && relay <= NUM_RELAYS) {
        return gpio_get(RELAY_PINS[relay - 1]) == 0; // Retorna true se o relé estiver ligado
    }
    return false;
}
