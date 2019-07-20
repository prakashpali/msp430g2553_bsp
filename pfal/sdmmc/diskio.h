/*-----------------------------------------------------------------------
/  PFF - Low level disk interface modlue include file    (C)ChaN, 2010
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include <msp430g2553.h>
#include "../spi/spi.h"
#include "../inc_16/std_types.h"
#include "../uart/uart.h"
#include "pff.h"

//
//
//
//            MSP430g2553                  MMC Card
//         -----------------          -----------------
//     /|\|              XIN|-   /|\ |                 |
//      | |                 |     |  |                 |
//      --|RST          XOUT|-    |--|Pin4/Vcc         |
//        |                 |        |                 |
//        |                 |        |                 |
//        |            P2.5 |<-------|Pin6/CD          |
//        |            P1.4 |------->|Pin1/CS          |
//        |                 |        |                 |
//        |      P1.6/SOMI  |------->|Pin7/DOUT        |
//        |      P1.7/SIMO  |<-------|Pin2/DIN         |
//        |      P1.5/UCLK  |------->|Pin5/CLK         |
//        |                 |        |                 |
//        |                 |     |--|Pin3/GND         |
//                                |
//                                =
//
//  Pin configuration at MSP430G2553:
//  --------------------------------
//  MSP430G2553      MSP Pin        MMC             MMC Pin
//  -------------------------------------------------------------
//  P1.4              48           ChipSelect       1
//  P1.6 / SOMI       46           DataOut          7
//                                 GND              3 (0 V)
//                                 VDD              4 (3.3 V)
//  P1.5 / UCLK       47           Clock            5
//  P2.5              44           CardDetect       6
//  P1.7 / SIMO       45           DataIn           2
//  -------------------------------------------------------------

/* Status of Disk Functions */
typedef BYTE	DSTATUS;

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */


/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Function succeeded */
	RES_ERROR,		/* 1: Disk error */
	RES_NOTRDY,		/* 2: Not ready */
	RES_PARERR		/* 3: Invalid parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (void);
DRESULT disk_readp (BYTE *buff, DWORD sector, UINT offset, UINT count);
DRESULT disk_writep (const BYTE*, DWORD);


#ifdef __cplusplus
}
#endif

#endif	/* _DISKIO_DEFINED */
