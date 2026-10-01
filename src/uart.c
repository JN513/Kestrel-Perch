#include "uart.h"


bool uart_available(int port)
{
    if (port < 4) {
        return uart_pio_available(port);
    }

    if (port == 4) {
        return uart_is_readable(uart1);
    }

    if (port == 5) {
        return uart_is_readable(uart0);
    }

    return false;
}

bool uart_getc_nonblocking(int port, uint8_t *c)
{
    if (port < 4) {
        return uart_pio_getc(port, c);
    }

    if (port == 4) {
        if (!uart_is_readable(uart1))
            return false;

        *c = uart_getc(uart1);
        return true;
    }

    if (port == 5) {
        if (!uart_is_readable(uart0))
            return false;

        *c = uart_getc(uart0);
        return true;
    }

    return false;
}

bool uart_tx_ready(int port)
{
    if (port < 4) {
        return uart_pio_tx_ready(port);
    }

    if (port == 4) {
        return !uart_is_writable(uart1);
    }

    if (port == 5) {
        return !uart_is_writable(uart0);
    }

    return false;
}

bool uart_putc_nonblocking(int port, uint8_t c)
{
    if (port < 4) {
        return uart_pio_putc(port, c);
    }

    if (port == 4) {
        uart_putc_raw(uart1, c);
        return true;
    }

    if (port == 5) {
        //if (!uart_is_writable(uart0))
        //    return false;

        uart_putc_raw(uart0, c);
        return true;
    }

    return false;
}


void init_uarts() {
    uart_init(uart1, config.baudrates[4]);
    gpio_set_function(config.tx_pins[4], GPIO_FUNC_UART);
    gpio_set_function(config.rx_pins[4], GPIO_FUNC_UART);

    uart_init(uart0, config.baudrates[5]);
    gpio_set_function(config.tx_pins[5], GPIO_FUNC_UART);
    gpio_set_function(config.rx_pins[5], GPIO_FUNC_UART);

    pio_uart_init();
}