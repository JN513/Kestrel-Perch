#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "config.h"

#include "pico/stdlib.h"
#include "bsp/board.h" // Necessário por causa da biblioteca tinyusb_board
#include "tusb.h"

#include "uart.h"
#include "command.h"
#include "power.h"
#include "pico/multicore.h"

#define LED_PIN PICO_DEFAULT_LED_PIN

void core1_main() {
    while (1) {
        for (int port = 0; port < NUM_PORTS; port++) {
            uint8_t ch;

            if (uart_getc_nonblocking(port, &ch) && tud_cdc_n_connected(port)) {
                tud_cdc_n_write_char(port, ch);
                tud_cdc_n_write_flush(port);
            }
        }
    }
}

int main(void) {
    board_init();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // Acende o LED para indicar que o sistema está inicializando

    init_power();
    init_uarts(); // Inicializa os UARTs
    
    // Inicializa a pilha TinyUSB
    tusb_init();

    multicore_launch_core1(core1_main);

    while (1) {
        tud_task();

        for (int port = 0; port < NUM_PORTS; port++) {
            if (tud_cdc_n_connected(port) && tud_cdc_n_available(port)) {
                uint8_t ch = tud_cdc_n_read_char(port);
                uart_putc_nonblocking(port, ch);
            }
        }

        handle_cmd_interface();
    }

    return 0;
}