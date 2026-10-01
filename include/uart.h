#ifndef __UART_H__
#define __UART_H__

#include "config.h"
#include "pio_uart.h"
#include "hardware/gpio.h"
#include "hardware/uart.h"

bool uart_available(int port);

bool uart_getc_nonblocking(int port, uint8_t *c);

bool uart_tx_ready(int port);

bool uart_putc_nonblocking(int port, uint8_t c);

void init_uarts();

#endif // !__UART_H__