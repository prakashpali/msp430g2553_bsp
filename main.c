/**
 * main.c
 * Set clock to 1MHz for the configurations here to work.
 *
 */

#include <msp430g2553.h>
#include <pfal/adc/adc.h>
#include "pfal/uart/uart.h"
#include "pfal/timer/timer.h"
#include "pfal/spi/spi.h"
#include "pfal/sdmmc/sdmmc.h"
#include "pfal/sdmmc/diskio.h"
//#include "pfal/i2c/i2c.h"

volatile uint8_t *uartTxData = "UART TX data\r\n";
volatile uint8_t uartRxData[50];
uint8_t spiRxData[50];
uint8_t spiTxData[50] = "SPI data\r\n";

volatile uint16_t time_us = 0;
volatile uint16_t time_ms = 0;
volatile uint16_t time_s = 0;
volatile uint16_t time_m = 0;
volatile uint16_t time_h = 0;

void main(void)
{
    uint16_t print_time = 0;
    uint8_t ret = 0;

	WDTCTL = WDTPW | WDTHOLD;		           // stop watchdog timer
    DCOCTL = 0;                                // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_16MHZ;                    // Set range
    DCOCTL = CALDCO_16MHZ;                     // Set DCO step + modulation

    UCA0MCTL = UCBRS2 + UCBRS0;                // Modulation UCBRSx = 5
    UCA0CTL1 &= ~UCSWRST;                      // Initialize USCI state machine

	//initSoftwareUart();                        // For debug prints over SW UART
	initHardwareUart();                        // For RX and TX over HW UART
    printf("Main started\r\n");

	initTimer();
	initTempSensor();

    ret = disk_initialize();
    printf_1("disk_initialize returned %d \r\n", ret);

    ret = disk_readp(spiRxData, 10, 0, 5);
    printf_1("disk_readp returned %d \r\n", ret);

//	ret = sdmmcInit();
//	printf_1("SDMMC Init returned %d \r\n", ret);
//
//	ret = sdmmcPing();
//    printf_1("SDMMC Ping returned %d \r\n", ret);
//
//    ret = sdmmcCheckBusy();
//    printf_1("SDMMC Check Busy returned %d \r\n", ret);
//
//    ret = sdmmcReadCardSize();
//    printf_1("SDMMC Card Size returned %d \r\n", ret);
//
//    ret = sdmmcWriteBlock (10, 5, spiTxData);
//    printf_1("SDMMC Write block returned %d \r\n", ret);
//
//    ret = sdmmcReadBlock(10, 5, spiRxData);
//    printf_1("SDMMC Read block returned %d \r\n", ret);



	//initSpi();
	//sendSpiData(spiTxData, 10);

    //printf_1("Size of short int is %d \r\n", sizeof(short int));

    __bis_SR_register(GIE);       // Enter LPM0, interrupts enabled

    while(1)
    {
        if( print_time ^ (time_s & 0x0001))
        {
            printf_3("Time - %d hr %d m %d s, ", time_h, time_m, time_s);
            printf_1("Temp - %d       \r", getTemp());
            print_time = print_time ^ 1;

        }

    }

}


