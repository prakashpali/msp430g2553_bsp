/*
 * uart.h
 *
 *  Created on: Jun 19, 2019
 *      Author: ppali
 */

#ifndef UART_H
#define UART_H

#include <msp430g2553.h>
#include "../inc_16/std_types.h"

#define UART_TX_PIN P1OUT
#define UART_TX_PIN_NO 0
#define UART_TX_PIN_SET(x) UART_TX_PIN = x << UART_TX_PIN_NO

#define UART_BAUD_RATE 115200 //38400

uint16_t convertToUartBits(uint8_t c);
void swUartSendChar(uint8_t c);
void sendChar(uint8_t c);
void sendStr(const uint8_t *c);
void initSoftwareUart(void);
void printf(const uint8_t*c);
void initHardwareUart(void);
uint8_t*convert(uint16_t num, uint16_t base);
void printf_1(const uint8_t*c, uint16_t val);
void printf_2(const uint8_t*c, uint16_t val1, uint16_t val2);
void printf_3(const uint8_t*c, uint16_t val1, uint16_t val2, uint16_t val3);

#endif  /* #ifndef UART_H */
