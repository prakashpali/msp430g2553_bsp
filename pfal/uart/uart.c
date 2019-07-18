/*
 * uart.c
 *
 *  Created on: Jun 19, 2019
 *      Author: ppali
 *
 *      P1.1 - RX
 *      P1.2 - TX
 */

#include "uart.h"

volatile uint8_t count = 0;
volatile uint16_t raw_uart_data = 0x0;

extern volatile uint8_t *uartTxData;
extern volatile uint8_t uartRxData[50];
volatile uint8_t rxCount = 0;
volatile uint8_t txCount = 0;

void initSoftwareUart(void)
{

        //    1Mhz
        if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
        {
            while(1);                             // do not load, trap CPU!!
        }

        P1DIR |= 0x01;                              // configure P1.0 as output
        UART_TX_PIN_SET(1);

        CCR0 = 1000000/UART_BAUD_RATE - 1;          //104-1; //833-1;
        TACTL = TASSEL_2 + MC_1;                    // SMCLK, upmode

}

void initHardwareUart(void)
{
        //    16Mhz
        if (CALBC1_16MHZ==0xFF)                    // If calibration constant erased
        {
            while(1);                             // do not load, trap CPU!!
        }

        P1SEL |= BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
        P1SEL2 |= BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD

        UCA0CTL1 |= UCSSEL_2;                     // SMCLK
        UCA0BR0 = 138;                            // 16MHz 9600
        UCA0BR1 = 0;                              // 16MHz 9600
        UCA0MCTL = UCBRS_7;                        // Modulation UCBRSx = 0x0c
        UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
//        while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
//        UCA0TXBUF = 0x41;                         // TX -> RXed character

}

void printf(const uint8_t *c)
{
    sendStr(c);
}

void printf_1(const uint8_t *c, uint16_t val)
{
    uint8_t *traverse;
    int16_t i;

    for(traverse = (uint8_t *)c; *traverse != '\0'; traverse++)
        {

            if (*traverse == '%')
            {
                traverse++;

                //Module 2: Fetching and executing arguments
                switch(*traverse)
                {
                    case 'c' : i = val;     //Fetch uint8_t argument
                                sendChar(i);
                                break;

                    case 'd' : i = val;         //Fetch Decimal/Integer argument
                                if(i<0)
                                {
                                    i = -i;
                                    sendChar('-');
                                }
                                sendStr(convert(i,10));
                                break;

                    case 'o': i = val; //Fetch Octal representation
                                sendStr(convert(i,8));
                                break;

    //                case 's': i = val;       //Fetch string
    //                            sendStr((uint8_t *)i);
    //                            break;

                    case 'x': i = val; //Fetch Hexadecimal representation
                                sendChar('0');
                                sendChar('x');
                                //sendChar('0');
                                sendStr(convert(i,16));
                                break;
                }

                traverse++;

            }

            sendChar(*traverse);

        }
}

void printf_2(const uint8_t *c, uint16_t val1, uint16_t val2)
{
    uint8_t *traverse;
    int16_t i;

    for(traverse = (uint8_t *)c; *traverse != '\0'; traverse++)
        {

        if (*traverse == '%')
        {
            traverse++;

            //Module 2: Fetching and executing arguments
            switch(*traverse)
            {
                case 'c' : i = val1;     //Fetch uint8_t argument
                            sendChar(i);
                            break;

                case 'd' : i = val1;         //Fetch Decimal/Integer argument
                            if(i<0)
                            {
                                i = -i;
                                sendChar('-');
                            }
                            sendStr(convert(i,10));
                            break;

                case 'o': i = val1; //Fetch Octal representation
                            sendStr(convert(i,8));
                            break;

//                case 's': i = val1;       //Fetch string
//                            sendStr((uint8_t *)i);
//                            break;

                case 'x': i = val1; //Fetch Hexadecimal representation
                            sendChar('0');
                            sendChar('x');
                            //sendChar('0');
                            sendStr(convert(i,16));
                            break;
            }

            traverse++;
            val1 = val2;

        }

        sendChar(*traverse);

        }
}


void printf_3(const uint8_t *c, uint16_t val1, uint16_t val2, uint16_t val3)
{
    uint8_t *traverse;
    int16_t i;

    for(traverse = (uint8_t *)c; *traverse != '\0'; traverse++)
        {

        if (*traverse == '%')
        {
            traverse++;

            //Module 2: Fetching and executing arguments
            switch(*traverse)
            {
                case 'c' : i = val1;     //Fetch uint8_t argument
                            sendChar(i);
                            break;

                case 'd' : i = val1;         //Fetch Decimal/Integer argument
                            if(i<0)
                            {
                                i = -i;
                                sendChar('-');
                            }
                            sendStr(convert(i,10));
                            break;

                case 'o': i = val1; //Fetch Octal representation
                            sendStr(convert(i,8));
                            break;

//                case 's': i = val1;       //Fetch string
//                            sendStr((uint8_t *)i);
//                            break;

                case 'x': i = val1; //Fetch Hexadecimal representation
                            sendChar('0');
                            sendChar('x');
                            //sendChar('0');
                            sendStr(convert(i,16));
                            break;
            }

            traverse++;
            val1 = val2;
            val2 = val3;

        }

        sendChar(*traverse);

        }
}


uint8_t *convert(uint16_t num, uint16_t base)
{
    static uint8_t Representation[]= "0123456789ABCDEF";
    static uint8_t buffer[50];
    uint8_t *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do
    {
        *--ptr = Representation[num%base];
        num /= base;
    }while(num != 0);

    return(ptr);
}

void sendStr(const uint8_t *c)
{

    do
    {
        sendChar(*c);
        c++;
    } while(*c != '\0');

//    sendChar('\r');
//    sendChar('\n');

}


void sendChar(uint8_t c)
{
    UC0IE |= UCA0TXIE; // Enable USCI_A0 TX interrupt

    while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?

    UCA0TXBUF = c;

    UC0IE &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt

}

void swUartSendChar(uint8_t c)
{
    uint16_t i;
    raw_uart_data = convertToUartBits(c);

    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    //__bis_SR_register(GIE);       // Enter LPM0 w/ interrupt

    for(i = 0; i < 1000; i++);

}

uint16_t convertToUartBits(uint8_t c)
{
    uint16_t data = 0;
    data = c;
    data = data << 1;
    data |= 1<<9;       // Set stop bit

    return data;
}

/*
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  P1OUT = (raw_uart_data & 0x1) << UART_TX_PIN_NO;                            // Toggle P1.0
  raw_uart_data = raw_uart_data >> 1;
  if(!raw_uart_data)
  {
      CCTL0 = 0;
      //P1OUT |= 0x1;
  }

}
*/

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
   if (UCA0RXBUF)
   {

      uartRxData[rxCount++] = UCA0RXBUF;
      UCA0TXBUF = UCA0RXBUF;
      if (UCA0RXBUF == '\n'|| rxCount > 49)
      {
          rxCount = 0;
          UC0IE |= UCA0TXIE; // Enable USCI_A0 TX interrupt
      }
   }

}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
//   UCA0TXBUF = uartTxData[txCount++]; // TX next character
//   if (uartTxData[txCount] == '\n' || txCount > 49) // TX over?
//   {
//       txCount = 0;
//       UC0IE &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt
//   }

   UC0IE &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt

}

