/*
 * timer.c
 *
 *  Created on: Jun 19, 2019
 *      Author: ppali
 */
#include "timer.h"

extern volatile uint16_t time_ms;
extern volatile uint16_t time_s;
extern volatile uint16_t time_m;
extern volatile uint16_t time_h;

extern void printf_3(const char *c, uint16_t val1, uint16_t val2, uint16_t val3);

void initTimer(void)
{
    //Timer Configuration
    TA1CCR0 = 16000; //Initially, Stop the Timer
    TA1CCTL0 |= CCIE; //Enable interrupt for CCR0.
    TA1CTL = TASSEL_2 + ID_0 + MC_1; //Select SMCLK, SMCLK/1 , Up Mode
}

// Timer 1 A0 interrupt service routine
#pragma vector = TIMER1_A0_VECTOR
__interrupt void Timer1_A0_ISR( void )
{
    time_ms++;

    if(time_ms > 999)
        {
            time_s++;
            time_ms = 0;
            //printf_1("Time %d\r\n", time_s);
        }

    if(time_s > 59)
        {
            time_s = 0;
            time_m++;
        }

    if(time_m > 59)
        {
            time_m = 0;
            time_h++;
        }


}
