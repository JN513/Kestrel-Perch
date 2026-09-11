#ifndef __UART_H__
#define __UART_H__

#include "config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>



typedef struct {
    uint32_t baudrate;
    bool enabled;
} uart_config_t;

void uart_init(void);

bool uart_configure(
    uint8_t port,
    const uart_config_t *config
);

size_t uart_read(
    uint8_t port,
    uint8_t *buffer,
    size_t length
);

size_t uart_write(
    uint8_t port,
    const uint8_t *buffer,
    size_t length
);

#endif // !__UART_H__
