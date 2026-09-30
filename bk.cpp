#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h> // Essencial para o va_list do cli_printf

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/pio.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include "bsp/board_api.h" // Incluído para board_init()
#include "tusb.h"

#include "uart_tx_rx.pio.h"

// Memória Flash (Último setor de 4KB para substituir a EEPROM)
#define FLASH_TARGET_OFFSET (PICO_FLASH_SIZE_BYTES - FLASH_SECTOR_SIZE)
const uint8_t *flash_config_contents = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);

#define EEPROM_VALID_FLAG 0xA5
#define DEFAULT_BAUDRATE 9600

#define LED_PIN PICO_DEFAULT_LED_PIN
#define NUM_RELAYS 8
#define NUM_PORTS 6

const uint RELAY_PINS[NUM_RELAYS] = {21, 20, 19, 18, 17, 16, 15, 14};

// Estrutura de Configuração
typedef struct {
    uint8_t flag;
    int baudrates[NUM_PORTS];
    int tx_pins[NUM_PORTS];
    int rx_pins[NUM_PORTS];
} config_t;

config_t config;

// Definições de instâncias PIO
PIO pio_hw = pio0;
uint pio_offset_tx;
uint pio_offset_rx;
uint pio_sm_tx[4];
uint pio_sm_rx[4];

// Buffers do CLI Command interface
char cmdBuffer[128];
size_t cmdLen = 0;

// Declarações de Funções
void init_hardware();
void load_config();
void save_config();
void init_default_config();
void configure_ports();
void core1_entry();
void handle_cmd_interface();
void process_cmd(const char *cmd);
void print_help();

// --- Auxiliares PIO UART ---
void pio_uart_write(int pio_port_idx, uint8_t ch) {
    pio_sm_put_blocking(pio_hw, pio_sm_tx[pio_port_idx], ch);
}

bool pio_uart_read(int pio_port_idx, uint8_t *ch) {
    if (!pio_sm_is_rx_fifo_empty(pio_hw, pio_sm_rx[pio_port_idx])) {
        *ch = (uint8_t) pio_sm_get(pio_hw, pio_sm_rx[pio_port_idx]);
        return true;
    }
    return false;
}

// --- Envio de resposta para a CLI (CDC 6) ---
void cli_printf(const char *format, ...) {
    char buf[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);

    if (tud_cdc_n_connected(6)) {
        tud_cdc_n_write(6, buf, strlen(buf));
        tud_cdc_n_write_flush(6);
    }
}

int main() {
    // Inicializa placa, clocks do sistema, e GPIOs básicos
    board_init();
    stdio_init_all();

    // Inicializa a pilha TinyUSB e conecta o pull-up D+ na porta USB
    tusb_init();

    load_config();
    init_hardware();
    configure_ports();

    // Inicia o Core 1
    multicore_launch_core1(core1_entry);

    uint32_t last_blink = 0;
    bool led_state = false;

    while (1) {
        tud_task(); // Mantém o serviço USB ativo

        // Blink do LED (Heartbeat)
        if (to_ms_since_boot(get_absolute_time()) - last_blink > 500) {
            last_blink = to_ms_since_boot(get_absolute_time());
            led_state = !led_state;
            gpio_put(LED_PIN, led_state);
        }

        // Repassagem USB CDC -> UART
        for (int i = 0; i < NUM_PORTS; i++) {
            if (tud_cdc_n_connected(i) && tud_cdc_n_available(i)) {
                uint8_t ch = tud_cdc_n_read_char(i);
                
                if (i == 3) {
                    uart_putc(uart1, ch);
                } else if (i == 5) {
                    uart_putc(uart0, ch);
                } else {
                    int pio_idx = (i < 3) ? i : 3;
                    pio_uart_write(pio_idx, ch);
                }
            }
        }

        handle_cmd_interface();
    }
}

// --- CORE 1: UART -> USB Forwarding ---
void core1_entry() {
    // Habilita o Core 1 a ser pausado durante a gravação da Flash no Core 0
    multicore_lockout_victim_init();

    uint8_t ch;
    while (1) {
        // ACM0 (PIO 0)
        if (pio_uart_read(0, &ch) && tud_cdc_n_connected(0)) {
            tud_cdc_n_write_char(0, ch); tud_cdc_n_write_flush(0);
        }
        // ACM1 (PIO 1)
        if (pio_uart_read(1, &ch) && tud_cdc_n_connected(1)) {
            tud_cdc_n_write_char(1, ch); tud_cdc_n_write_flush(1);
        }
        // ACM2 (PIO 2)
        if (pio_uart_read(2, &ch) && tud_cdc_n_connected(2)) {
            tud_cdc_n_write_char(2, ch); tud_cdc_n_write_flush(2);
        }
        // ACM3 (HW UART1)
        if (uart_is_readable(uart1)) {
            ch = uart_getc(uart1);
            if (tud_cdc_n_connected(3)) { tud_cdc_n_write_char(3, ch); tud_cdc_n_write_flush(3); }
        }
        // ACM4 (PIO 3)
        if (pio_uart_read(3, &ch) && tud_cdc_n_connected(4)) {
            tud_cdc_n_write_char(4, ch); tud_cdc_n_write_flush(4);
        }
        // ACM5 (HW UART0)
        if (uart_is_readable(uart0)) {
            ch = uart_getc(uart0);
            if (tud_cdc_n_connected(5)) { tud_cdc_n_write_char(5, ch); tud_cdc_n_write_flush(5); }
        }
    }
}

void init_hardware() {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    for (int i = 0; i < NUM_RELAYS; i++) {
        gpio_init(RELAY_PINS[i]);
        gpio_set_dir(RELAY_PINS[i], GPIO_OUT);
        gpio_put(RELAY_PINS[i], 0);
    }

    // Carrega instruções no PIO
    pio_offset_tx = pio_add_program(pio_hw, &uart_tx_program);
    pio_offset_rx = pio_add_program(pio_hw, &uart_rx_program);

    for (int i = 0; i < 4; i++) {
        pio_sm_tx[i] = pio_claim_unused_sm(pio_hw, true);
        pio_sm_rx[i] = pio_claim_unused_sm(pio_hw, true);
    }
}

void configure_ports() {
    // Configura HW UARTs
    uart_init(uart1, config.baudrates[3]);
    gpio_set_function(config.tx_pins[3], GPIO_FUNC_UART);
    gpio_set_function(config.rx_pins[3], GPIO_FUNC_UART);

    uart_init(uart0, config.baudrates[5]);
    gpio_set_function(config.tx_pins[5], GPIO_FUNC_UART);
    gpio_set_function(config.rx_pins[5], GPIO_FUNC_UART);

    // Configura PIO UARTs (0, 1, 2, 4)
    int pio_indices[4] = {0, 1, 2, 4};
    for (int k = 0; k < 4; k++) {
        int idx = pio_indices[k];
        
        // Desativa a State Machine antes de reconfigurar (evita falhas de sincronia dinâmicas)
        pio_sm_set_enabled(pio_hw, pio_sm_tx[k], false);
        pio_sm_set_enabled(pio_hw, pio_sm_rx[k], false);

        uart_tx_program_init(pio_hw, pio_sm_tx[k], pio_offset_tx, config.tx_pins[idx], config.baudrates[idx]);
        uart_rx_program_init(pio_hw, pio_sm_rx[k], pio_offset_rx, config.rx_pins[idx], config.baudrates[idx]);
    }
}

// --- CLI / Terminal Commands ---
void handle_cmd_interface() {
    if (!tud_cdc_n_connected(6) || !tud_cdc_n_available(6)) return;

    while (tud_cdc_n_available(6)) {
        char ch = tud_cdc_n_read_char(6);
        
        if (ch == '\r' || ch == '\n') {
            cli_printf("\r\n");
            cmdBuffer[cmdLen] = '\0';
            if(cmdLen > 0) {
                process_cmd(cmdBuffer);
            }
            cmdLen = 0;
            cli_printf("# ");
        } else if ((ch == '\b' || ch == 127) && cmdLen > 0) {
            cmdLen--;
            cli_printf("\b \b");
        } else if (ch >= 32 && ch <= 126 && cmdLen < sizeof(cmdBuffer) - 1) {
            cmdBuffer[cmdLen++] = ch;
            tud_cdc_n_write_char(6, ch);
            tud_cdc_n_write_flush(6);
        }
    }
}

void print_help() {
    cli_printf("Comandos disponiveis:\r\n");
    cli_printf("  help                    - Mostra esta ajuda\r\n");
    cli_printf("  print_config            - Mostra as configuracoes atuais\r\n");
    cli_printf("  set_baud <acm> <baud>   - Altera o baudrate da porta ACM (0-5)\r\n");
    cli_printf("  reset_baud              - Reseta todos os baudrates\r\n");
    cli_printf("  set_pin <acm> <tx> <rx> - Altera os pinos da porta ACM (0-5)\r\n");
    cli_printf("  reset_pins              - Reseta os pinos aos padroes\r\n");
    cli_printf("  relay_on <1-8>          - Liga o rele\r\n");
    cli_printf("  relay_off <1-8>         - Desliga o rele\r\n");
    cli_printf("  relay_status <1-8|all>  - Checa o status dos reles\r\n");
}

void process_cmd(const char *cmd) {
    int relay, acmIndex, baud, txPin, rxPin;

    if (strcmp(cmd, "help") == 0) {
        print_help();

    } else if (strcmp(cmd, "reset_baud") == 0) {
        for (int i = 0; i < NUM_PORTS; i++) config.baudrates[i] = DEFAULT_BAUDRATE;
        save_config();
        configure_ports();
        cli_printf("Baud rates resetados.\r\n");

    } else if (sscanf(cmd, "set_baud %d %d", &acmIndex, &baud) == 2) {
        if (acmIndex >= 0 && acmIndex < NUM_PORTS) {
            config.baudrates[acmIndex] = baud;
            save_config();
            configure_ports();
            cli_printf("Baud rate da ACM%d configurado para %d.\r\n", acmIndex, baud);
        } else {
            cli_printf("Indice ACM invalido.\r\n");
        }

    } else if (sscanf(cmd, "set_pin %d %d %d", &acmIndex, &txPin, &rxPin) == 3) {
        if (acmIndex >= 0 && acmIndex < NUM_PORTS) {
            config.tx_pins[acmIndex] = txPin;
            config.rx_pins[acmIndex] = rxPin;
            save_config();
            configure_ports();
            cli_printf("Pinos da ACM%d configurados para TX:%d RX:%d\r\n", acmIndex, txPin, rxPin);
        } else {
             cli_printf("Indice ACM invalido.\r\n");
        }

    } else if (strcmp(cmd, "reset_pins") == 0) {
        init_default_config();
        save_config();
        configure_ports();
        cli_printf("Pinos resetados para o padrao.\r\n");

    } else if (strcmp(cmd, "print_config") == 0) {
        cli_printf("Configuracao Atual:\r\n");
        for (int i = 0; i < NUM_PORTS; i++) {
            cli_printf("ACM%d: Baud = %d, TX = %d, RX = %d\r\n", i, config.baudrates[i], config.tx_pins[i], config.rx_pins[i]);
        }

    } else if (sscanf(cmd, "relay_on %d", &relay) == 1) {
        if (relay >= 1 && relay <= NUM_RELAYS) {
            gpio_put(RELAY_PINS[relay - 1], 1);
            cli_printf("Rele %d: ON\r\n", relay);
        } else cli_printf("Rele invalido. Use 1-8.\r\n");

    } else if (sscanf(cmd, "relay_off %d", &relay) == 1) {
        if (relay >= 1 && relay <= NUM_RELAYS) {
            gpio_put(RELAY_PINS[relay - 1], 0);
            cli_printf("Rele %d: OFF\r\n", relay);
        } else cli_printf("Rele invalido. Use 1-8.\r\n");

    } else if (strcmp(cmd, "relay_status all") == 0) {
        cli_printf("Status dos reles:\r\n");
        for (int i = 0; i < NUM_RELAYS; i++) {
            cli_printf("  Rele %d: %s\r\n", i + 1, gpio_get(RELAY_PINS[i]) ? "ON" : "OFF");
        }

    } else if (sscanf(cmd, "relay_status %d", &relay) == 1) {
        if (relay >= 1 && relay <= NUM_RELAYS) {
            cli_printf("Rele %d: %s\r\n", relay, gpio_get(RELAY_PINS[relay - 1]) ? "ON" : "OFF");
        } else cli_printf("Rele invalido. Use 1-8.\r\n");

    } else {
        cli_printf("Comando desconhecido. Digite 'help' para a lista de comandos.\r\n");
    }
}

// --- Gerenciamento da Flash (Simulando a EEPROM) ---
void load_config() {
    memcpy(&config, flash_config_contents, sizeof(config_t));
    if (config.flag != EEPROM_VALID_FLAG) {
        init_default_config();
        save_config();
    }
}

void init_default_config() {
    config.flag = EEPROM_VALID_FLAG;
    int defaultTX[NUM_PORTS] = {2, 4, 6, 8, 10, 12};
    int defaultRX[NUM_PORTS] = {3, 5, 7, 9, 11, 13};

    for (int i = 0; i < NUM_PORTS; i++) {
        config.baudrates[i] = DEFAULT_BAUDRATE;
        config.tx_pins[i] = defaultTX[i];
        config.rx_pins[i] = defaultRX[i];
    }
}

void save_config() {
    uint8_t buffer[FLASH_SECTOR_SIZE];
    memset(buffer, 0xFF, FLASH_SECTOR_SIZE);
    memcpy(buffer, &config, sizeof(config_t));

    // OBRIGATORIO: Pausa o Core 1. A leitura de flash pelo Core 1 durante um erase causa crash!
    multicore_lockout_start_blocking();

    uint32_t ints = save_and_disable_interrupts();
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(FLASH_TARGET_OFFSET, buffer, FLASH_SECTOR_SIZE);
    restore_interrupts(ints);

    // Retorna execução do Core 1
    multicore_lockout_end_blocking();
}