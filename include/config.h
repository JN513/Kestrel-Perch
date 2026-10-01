#ifndef __CONFIG_H__
#define __CONFIG_H__

#pragma once

#define NUM_RELAYS 8
#define NUM_PORTS 6

#define DEFAULT_BAUDRATE    115200
#define NUM_PIO_UARTS 4

#define RELAY_1_PIN 21
#define RELAY_2_PIN 20
#define RELAY_3_PIN 19
#define RELAY_4_PIN 18
#define RELAY_5_PIN 17
#define RELAY_6_PIN 16
#define RELAY_7_PIN 15
#define RELAY_8_PIN 14

// /dev/ttyACM0 => 2 (TX); 3 (RX) | PIO
// /dev/ttyACM1 => 4 (TX); 5 (RX) | PIO
// /dev/ttyACM2 => 6 (TX); 7 (RX) | PIO
// /dev/ttyACM3 => 8 (TX); 9 (RX) | HW-UART
// /dev/ttyACM4 => 10 (TX); 11 (RX) | PIO
// /dev/ttyACM5 => 12 (TX); 13 (RX) | HW-UART

#define UART0_RX_PIN 3
#define UART0_TX_PIN 2

#define UART1_RX_PIN 5
#define UART1_TX_PIN 4

#define UART2_RX_PIN 7
#define UART2_TX_PIN 6

#define UART3_RX_PIN 9
#define UART3_TX_PIN 8

#define UART4_RX_PIN 11
#define UART4_TX_PIN 10

#define UART5_RX_PIN 13
#define UART5_TX_PIN 12

static const int defaultTX[NUM_PORTS] = {UART0_TX_PIN, UART1_TX_PIN, UART2_TX_PIN,
     UART3_TX_PIN, UART4_TX_PIN, UART5_TX_PIN};
static const int defaultRX[NUM_PORTS] = {UART0_RX_PIN, UART1_RX_PIN, UART2_RX_PIN,
     UART3_RX_PIN, UART4_RX_PIN, UART5_RX_PIN};

#define UART_RX_BUFFER_SIZE 4096
#define UART_TX_BUFFER_SIZE 4096


#endif // !__CONFIG_H__
