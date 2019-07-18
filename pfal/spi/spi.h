/*
 * spi.h
 *
 *  Created on: Jun 19, 2019
 *      Author: ppali
 */
#ifndef SPI_H
#define SPI_H

#include <msp430g2553.h>
#include "../inc_16/std_types.h"

//----------------------------------------------------------------------------
//  These constants are used to identify the chosen SPI and UART
//  interfaces.
//----------------------------------------------------------------------------
#define SER_INTF_NULL    0
#define SER_INTF_USART0  1
#define SER_INTF_USART1  2
#define SER_INTF_USCIA0  3
#define SER_INTF_USCIA1  4
#define SER_INTF_USCIB0  5
#define SER_INTF_USCIB1  6
#define SER_INTF_USI     7
#define SER_INTF_BITBANG 8

#define halSPIRXBUF UCB0RXBUF
#define halSPI_SEND(x) UCB0TXBUF=x
#define halSPITXREADY  (IFG2 & UCB0TXIFG)           /* Wait for TX to be ready */
#define halSPIRXREADY (IFG2 & UCB0RXIFG)            /* Wait for TX to be ready */
#define halSPITXDONE  (!(UCB0STAT & UCBUSY))        /* Wait for TX to finish */
#define halSPIRXFG_CLR (IFG2 &= UCA0RXIFG)
#define halSPI_PxIN  SPI_USART0_PxIN
#define halSPI_SOMI  SPI_USART0_SOMI

// Function Prototypes
void initSpi(void);
uint8_t spiSendByte(const uint8_t data);
uint8_t getSpiByte(void);
void sendSpiData(uint8_t *d, uint8_t num_bytes);
void getSpiData(uint8_t *d, uint8_t num_bytes);

#endif /* #ifndef SPI_H */
