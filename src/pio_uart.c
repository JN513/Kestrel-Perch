#include "pio_uart.h"


pio_uart_t pio_uarts[NUM_PIO_UARTS];

uint pio_offset_tx[2];
uint pio_offset_rx[2];

static inline int get_port(int port)
{
    switch (port) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 4: return 3;
        default: return -1;
    }
}

bool uart_pio_tx_ready(int port)
{
    int _port = get_port(port);

    if (_port < 0)
        return false;

    PIO pio = pio_uarts[_port].pio;
    uint sm = pio_uarts[_port].sm_tx;

    return !pio_sm_is_tx_fifo_full(pio, sm);
}

bool uart_pio_putc(int port, uint8_t c)
{
    int _port = get_port(port);
    if (_port < 0)
        return false;
    
    PIO pio = pio_uarts[_port].pio;
    uint sm = pio_uarts[_port].sm_tx;

    if (pio_sm_is_tx_fifo_full(pio, sm))
        return false;

    pio_sm_put(pio, sm, (uint32_t)c);

    return true;
}

bool uart_pio_available(int port)
{
    int _port = get_port(port);
    if (_port < 0)
        return false;

    PIO pio = pio_uarts[_port].pio;
    uint sm = pio_uarts[_port].sm_rx;

    return !pio_sm_is_rx_fifo_empty(pio, sm);
}

bool uart_pio_getc(int port, uint8_t *c)
{
    int _port = get_port(port);
    if (_port < 0)
        return false;
    
        PIO pio = pio_uarts[_port].pio;
    uint sm = pio_uarts[_port].sm_rx;

    if (pio_sm_is_rx_fifo_empty(pio, sm))
        return false;

    *c = (uint8_t)(pio_sm_get(pio, sm) >> 24);

    return true;
}


void pio_uart_init(void)
{
    PIO pios[2] = {
        pio0,
        pio1
    };

    // Carrega os programas TX e RX em cada PIO
    for (int p = 0; p < 2; p++) {

        pio_offset_tx[p] =
            pio_add_program(pios[p], &uart_tx_program);

        pio_offset_rx[p] =
            pio_add_program(pios[p], &uart_rx_program);
    }

    // UART 0 e 1 -> PIO0
    pio_uarts[0].pio = pio0;
    pio_uarts[1].pio = pio0;

    // UART 2 e 3 -> PIO1
    pio_uarts[2].pio = pio1;
    pio_uarts[3].pio = pio1;

    // State machines
    pio_uarts[0].sm_tx = 0;
    pio_uarts[0].sm_rx = 1;

    pio_uarts[1].sm_tx = 2;
    pio_uarts[1].sm_rx = 3;

    pio_uarts[2].sm_tx = 0;
    pio_uarts[2].sm_rx = 1;

    pio_uarts[3].sm_tx = 2;
    pio_uarts[3].sm_rx = 3;

    // Inicializa cada UART
    for (int i = 0; i < NUM_PIO_UARTS - 1; i++) {

        int p = (i < 2) ? 0 : 1;

        uart_tx_init(
            pio_uarts[i].pio,
            pio_uarts[i].sm_tx,
            pio_offset_tx[p],
            defaultTX[i],
            DEFAULT_BAUDRATE
        );

        uart_rx_init(
            pio_uarts[i].pio,
            pio_uarts[i].sm_rx,
            pio_offset_rx[p],
            defaultRX[i],
            DEFAULT_BAUDRATE
        );
    }

    uart_tx_init(
        pio_uarts[3].pio,
        pio_uarts[3].sm_tx,
        pio_offset_tx[1],
        defaultTX[4],
        DEFAULT_BAUDRATE
    );

    uart_rx_init(
        pio_uarts[3].pio,
        pio_uarts[3].sm_rx,
        pio_offset_rx[1],
        defaultRX[4],
        DEFAULT_BAUDRATE
    );
}