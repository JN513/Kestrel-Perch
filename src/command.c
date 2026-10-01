#include "command.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h> 

#include "tusb.h"
#include "power.h"

// Buffers do CLI Command interface
char cmdBuffer[128];
size_t cmdLen = 0;


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

void process_cmd(const char *cmd) {
    int relay;

    if (sscanf(cmd, "relay_on %d", &relay) == 1) {
        if (relay >= 1 && relay <= NUM_RELAYS) {
            power_on_relay(relay);
            cli_printf("ON\r\n");
        } else cli_printf("-1\r\n");

    } else if (sscanf(cmd, "relay_off %d", &relay) == 1) {
        if (relay >= 1 && relay <= NUM_RELAYS) {
            power_off_relay(relay);
            cli_printf("OFF\r\n");
        } else cli_printf("-1\r\n");

    } else if (sscanf(cmd, "relay_status %d", &relay) == 1) {
        if (relay >= 1 && relay <= NUM_RELAYS) {
            cli_printf("%s\r\n", is_relay_on(relay) ? "ON" : "OFF");
        } else cli_printf("-1\r\n");

    } else {
        cli_printf("-1\r\n");
    }
}

void handle_cmd_interface() {
    if (!tud_cdc_n_connected(6) || !tud_cdc_n_available(6)) return;

    while (tud_cdc_n_available(6)) {
        char ch = tud_cdc_n_read_char(6);
        
        if (ch == '\r' || ch == '\n') {
            cmdBuffer[cmdLen] = '\0';
            if(cmdLen > 0)
                process_cmd(cmdBuffer);
            cmdLen = 0;
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