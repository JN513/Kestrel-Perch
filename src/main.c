#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h> 

#include "pico/stdlib.h"
#include "bsp/board.h" // Necessário por causa da biblioteca tinyusb_board
#include "tusb.h"

#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"

#define LED_PIN PICO_DEFAULT_LED_PIN
#define NUM_RELAYS 8
#define NUM_PORTS 6
#define DEFAULT_BAUDRATE 115200

const uint RELAY_PINS[NUM_RELAYS] = {21, 20, 19, 18, 17, 16, 15, 14};

typedef struct {
    int baudrates[NUM_PORTS];
    int tx_pins[NUM_PORTS];
    int rx_pins[NUM_PORTS];
} config_t;

config_t config;

void init_default_config() {
    int defaultTX[NUM_PORTS] = {2, 4, 6, 8, 10, 12};
    int defaultRX[NUM_PORTS] = {3, 5, 7, 9, 11, 13};

    for (int i = 0; i < NUM_PORTS; i++) {
        config.baudrates[i] = DEFAULT_BAUDRATE;
        config.tx_pins[i] = defaultTX[i];
        config.rx_pins[i] = defaultRX[i];
    }
}

void core1_main() {
    while (1) {

        if (uart_is_readable(uart1)) {
            uint8_t ch = uart_getc(uart1);
            if (tud_cdc_n_connected(4)) { tud_cdc_n_write_char(4, ch); tud_cdc_n_write_flush(4); }
        }

        if (uart_is_readable(uart0)) {
            uint8_t ch = uart_getc(uart0);
            if (tud_cdc_n_connected(5)) { tud_cdc_n_write_char(5, ch); tud_cdc_n_write_flush(5); }
        }
        sleep_ms(100); // Apenas para evitar um loop muito rápido
    }
}

int main(void) {
    // 1. Inicializa os clocks e o hardware USB da placa (Crucial!)
    board_init();

    init_default_config(); // Inicializa a configuração padrão

    uart_init(uart1, config.baudrates[4]);
    gpio_set_function(config.tx_pins[4], GPIO_FUNC_UART);
    gpio_set_function(config.rx_pins[4], GPIO_FUNC_UART);

    uart_init(uart0, config.baudrates[5]);
    gpio_set_function(config.tx_pins[5], GPIO_FUNC_UART);
    gpio_set_function(config.rx_pins[5], GPIO_FUNC_UART);
    
    // 2. Inicializa a pilha TinyUSB
    tusb_init();

    multicore_launch_core1(core1_main);

    // 3. Loop infinito estritamente dedicado ao USB
    while (1) {
        tud_task(); // Mantém o USB vivo. NENHUM CÓDIGO BLOQUEANTE PODE IR AQUI!

        for(int i = 0; i < 4; i++) {
            if(tud_cdc_n_connected(i) && tud_cdc_n_available(i)) {
                uint8_t ch = tud_cdc_n_read_char(i);
                tud_cdc_n_write_char(i, ch); // Ecoa o caractere de volta
                tud_cdc_n_write_flush(i);    // Garante que o caractere seja enviado imediatamente
            }
        }

        if(tud_cdc_n_connected(4) && tud_cdc_n_available(4)) {
            uint8_t ch = tud_cdc_n_read_char(4);
            uart_putc(uart1, ch);
        }

        if(tud_cdc_n_connected(5) && tud_cdc_n_available(5)) {
            uint8_t ch = tud_cdc_n_read_char(5);
            uart_putc(uart0, ch);
        }
    }

    return 0;
}