/*
 * spi.c
 *
 *  Created on: Jun 19, 2019
 *      Author: ppali
 *
 *      UCB0
 *      P1.6 – UCB0SOMI
 *      P1.7 – UCB0SIMO
 *      P1.5 – UCB0CLK
 *      P1.4 – SS
 *
 *
 *
 */

#include <msp430g2553.h>
#include "spi.h"

extern unsigned volatile char spiRxData[50];
extern unsigned volatile char spiTxData[50];
uint8_t spiRxCount = 0;
uint8_t spiTxCount = 0;
uint8_t MST_Data = 1, SLV_Data = 0;

void initSpi(void)
{

    //    16Mhz
    if (CALBC1_16MHZ==0xFF)                    // If calibration constant erased
    {
        while(1);                             // do not load, trap CPU!!
    }

//    UCB0CTL1 |= UCSWRST;
    UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC; // 3-pin, 8-bit SPI master
    UCB0CTL1 |= UCSSEL_2; // SMCLK
    UCB0BR0 |= 0x10; // /8
    UCB0BR1 = 0; //
    UCA0MCTL = 0; // No modulation
    UCB0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
    //IE2 |= UCB0RXIE;                          // Enable USCI0 RX interrupt

    P1DIR |= BIT4;
    P1OUT |= BIT4;

    P1SEL |= BIT5 | BIT6 | BIT7;
    P1SEL2 |= BIT5 | BIT6 | BIT7;

}

//Send one byte via SPI
uint8_t spiSendByte(const uint8_t data)
{
  while (halSPITXREADY == 0);    // wait while not ready for TX
  halSPI_SEND(data);            // write
//  while (halSPIRXREADY == 0);    // wait for RX buffer (full)
//  return (halSPIRXBUF);
  return 1;
}

void sendSpiData(uint8_t *d, uint8_t num_bytes)
{
    if (num_bytes < 50)
    {
        //P1OUT &= ~BIT4;
        while(num_bytes--)
        {
            while (!(IFG2 & UCB0TXIFG)); // USCI_A0 TX buffer ready?
            __delay_cycles(50);
            spiSendByte(d[spiTxCount++]); // Send data over SPI to Slave

        }
        spiTxCount = 0;
    }
    //P1OUT |= BIT4;
}

uint8_t getSpiByte(void)
{
    uint8_t b;
    //P1OUT &= ~BIT4;

    while (!(IFG2 & UCB0RXIFG)); // USCI_A0 RX Received?
    b = UCB0RXBUF; // Store received data

    //P1OUT |= BIT4;

    return b;
}

void getSpiData(uint8_t *d, uint8_t num_bytes)
{
    if (num_bytes < 50)
    {
        //P1OUT &= ~BIT4;
        while(num_bytes--)
        {
            while (!(IFG2 & UCB0RXIFG)); // USCI_A0 RX Received?
            d[spiRxCount++] = UCB0RXBUF; // Store received data

        }

        spiRxCount = 0;
    }
    //P1OUT |= BIT4;
}

//
//// Test for valid RX and TX character
//#pragma vector=USCIAB0RX_VECTOR
//__interrupt void USCIB0RX_ISR(void)
//{
//  volatile unsigned int i;
//
//  while (!(IFG2 & UCA0TXIFG));              // USCI_A0 TX buffer ready?
//
//  if (UCA0RXBUF == SLV_Data)                // Test for correct character RX'd
//    P1OUT |= BIT0;                          // If correct, light LED
//  else
//    P1OUT &= ~BIT0;                         // If incorrect, clear LED
//
//  MST_Data++;                               // Increment master value
//  SLV_Data++;                               // Increment expected slave value
//  UCA0TXBUF = MST_Data;                     // Send next value
//
//  __delay_cycles(50);                     // Add time between transmissions to
//}                                           // make sure slave can keep up
