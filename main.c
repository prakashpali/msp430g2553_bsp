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
#include "pfal/sdmmc/diskio.h"
//#include "pfal/i2c/i2c.h"

volatile uint8_t *uartTxData = "UART TX data\r\n";
volatile uint8_t uartRxData[50];
volatile uint8_t spiRxData[50];
volatile uint16_t spi_rx_count = 0;
uint8_t spiTxData[50] = "SPI data\r\n";

volatile uint16_t time_us = 0;
volatile uint16_t time_ms = 0;
volatile uint16_t time_s = 0;
volatile uint16_t time_m = 0;
volatile uint16_t time_h = 0;

void main(void)
{

    FATFS fatfs;            /* File system object */
//    DIR dir;                /* Directory object */
//    FILINFO fno;            /* File information object */
    UINT bw, br, i;
    BYTE buff[64];
    FRESULT rc;

    uint16_t print_time = 0;
    //uint8_t ret = 0;

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
/*
    ret = disk_initialize();
    printf_1("disk_initialize returned %d \r\n", ret);

    ret = disk_writep (spiTxData, 10);
    printf_1("disk_writep returned %d \r\n", ret);

    ret = disk_readp(spiRxData, 10, 0, 5);
    printf_1("disk_readp returned %d \r\n", ret);
*/

#if 1

    printf("\r\nMount a volume.\r\n");
    rc = pf_mount(&fatfs);
    printf_1("pf_mount returned %d \r\n", rc);

    printf("\r\nOpen a test file (message.txt).\r\n");
    rc = pf_open("MESSAGE.TXT");
    printf_1("pf_open returned %d \r\n", rc);

    printf("\r\nType the file content.\r\n");
    for (;;) {
        rc = pf_read(buff, sizeof(buff), &br);  /* Read a chunk of file */
        printf_1("pf_read returned %d \r\n", rc);
        if (rc || !br) break;           /* Error or end of file */
        for (i = 0; i < br; i++)        /* Type the data */
            printf_1("data is %d\r\n", buff[i]);
    }


    printf("\r\nOpen a file to write (write.txt).\r\n");
    rc = pf_open("WRITE.TXT");
    printf_1("pf_open returned %d \r\n", rc);

    printf("\r\nWrite a text data. (Hello world!)\r\n");
    for (;;) {
        rc = pf_write("Hello world!\r\n", 14, &bw);
        printf_1("pf_write returned %d \r\n", rc);
        if (rc || !bw) break;
    }

    printf("\r\nTerminate the file write process.\r\n");
    rc = pf_write(0, 0, &bw);
    printf_1("pf_write returned %d \r\n", rc);

#endif

    //printf("\r\nTest completed.\r\n");

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


