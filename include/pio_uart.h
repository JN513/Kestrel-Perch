#ifndef __PIO_UART_H__
#define __PIO_UART_H__

#include "config.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "uart_tx.pio.h"
#include "uart_rx.pio.h"


typedef struct {
    PIO pio;
    uint sm_tx;
    uint sm_rx;
} pio_uart_t;

bool uart_pio_tx_ready(int port);

bool uart_pio_putc(int port, uint8_t c);

bool uart_pio_available(int port);

bool uart_pio_getc(int port, uint8_t *c);

void pio_uart_init(void);

#endif // !__PIO_UART_H__
