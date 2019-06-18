#include <msp430.h>				


/**
 * main.c
 * Set clock to 1MHz for the configurations here to work.
 *
 */

#define UART_TX_PIN P1OUT
#define UART_TX_PIN_NO 0
#define UART_TX_PIN_SET(x) UART_TX_PIN = x << UART_TX_PIN_NO

#define UART_BAUD_RATE 9600 //38400

volatile char count = 0;
volatile int raw_uart_data = 0x0;

int convertToUartBits(char c);
void sendChar(char c);
void sendStr(const char *c);

void main(void)
{
    int i;

	WDTCTL = WDTPW | WDTHOLD;		// stop watchdog timer

//    1Mhz
    if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
    {
        while(1);                             // do not load, trap CPU!!
    }
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set range
    DCOCTL = CALDCO_1MHZ;                     // Set DCO step + modulation

//    8Mhz
//    if (CALBC1_8MHZ==0xFF)                    // If calibration constant erased
//    {
//      while(1);                               // do not load, trap CPU!!
//    }
//    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
//    BCSCTL1 = CALBC1_8MHZ;                    // Set DCO to 8MHz
//    DCOCTL = CALDCO_8MHZ;

//    16Mhz
//    if (CALBC1_16MHZ==0xFF)                   // If calibration constant erased
//    {
//    while(1);                                 // do not load, trap CPU!!
//    }
//    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
//    BCSCTL1 = CALBC1_16MHZ;                   // Set range
//    DCOCTL = CALDCO_16MHZ;                    // Set DCO step + modulation

	P1DIR |= 0x01;					            // configure P1.0 as output
	UART_TX_PIN_SET(1);

    CCR0 = 1000000/UART_BAUD_RATE - 1;          //104-1; //833-1;
    TACTL = TASSEL_2 + MC_1;                    // SMCLK, upmode
    __bis_SR_register(GIE);                     // Enter LPM0 w/ interrupt

    for(;;)
        {
            sendStr("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\n\r");
            for(i = 0; i < 10000; i++);

        }

//    for(;;)
//    {
//        sendStr("|\r");
//        for(i = 0; i < 1000; i++);
//        sendStr("/\r");
//        for(i = 0; i < 1000; i++);
//        sendStr("-\r");
//        for(i = 0; i < 1000; i++);
//        sendStr("\\\r");
//        for(i = 0; i < 1000; i++);
//        sendStr("|\r");
//        for(i = 0; i < 1000; i++);
//        sendStr("/\r");
//        for(i = 0; i < 1000; i++);
//        sendStr("-\r");
//        for(i = 0; i < 1000; i++);
//        sendStr("\\\r");
//        for(i = 0; i < 1000; i++);
//    }

}

void sendStr(const char *c)
{

    do
    {
        sendChar(*c);
        c++;
    } while(*(c - 1) != '\r');
}


void sendChar(char c)
{
    int i;
    raw_uart_data = convertToUartBits(c);

    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    //__bis_SR_register(GIE);       // Enter LPM0 w/ interrupt

    for(i = 0; i < 1000; i++);

}

int convertToUartBits(char c)
{
    int data = 0;
    data = c;
    data = data << 1;
    data |= 1<<9;       // Set stop bit

    return data;
}



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
