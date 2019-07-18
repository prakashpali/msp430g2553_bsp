/*
 * temp.c
 *
 *  Created on: Jun 19, 2019
 *      Author: ppali
 */


#include <pfal/adc/adc.h>

void initTempSensor(void)
{
    ADC10CTL0=SREF_1 + REFON + ADC10ON + ADC10SHT_3 ;   //1.5V ref,Ref on,64 clocks for sample
    ADC10CTL1=INCH_10+ ADC10DIV_3;                      //temp sensor is at 10 and clock/4
    __delay_cycles(1000);                               //wait 4 ref to settle

}

uint32_t getTemp(void)
{
    uint32_t t=0;
    uint8_t i = 0;

    for (i = 0; i < 5; i++)
    {
         ADC10CTL0 |= ENC + ADC10SC;                    //enable conversion and start conversion
         while(ADC10CTL1 & BUSY);                       //wait..i am converting..pum..pum..
         t += ADC10MEM;                                 //store val in t
         ADC10CTL0&=~ENC;                               //disable adc conv
    }

    t = t/5;
    return ((t - 673) * 423) / 1024;                   //convert and pass

}

