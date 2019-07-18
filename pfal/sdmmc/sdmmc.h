/*
 * sdmmc.h
 *
 *  Created on: Jul 13, 2019
 *      Author: ppali
 */

#ifndef PFAL_SDMMC_SDMMC_H_
#define PFAL_SDMMC_SDMMC_H_

#include <msp430g2553.h>
#include "../spi/spi.h"
#include "../inc_16/std_types.h"

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

// Chip Select
#define SDMMC_CS_PxOUT      P1OUT
#define SDMMC_CS_PxDIR      P1DIR
#define SDMMC_CS            0x10

// Card Detect
#define SDMMC_CD_PxIN       P2IN
#define SDMMC_CD_PxDIR      P2DIR
#define SDMMC_CD            0x20


// macro defines
#define HIGH(a) ((a>>8)&0xFF)               // high byte from word
#define LOW(a)  (a&0xFF)                    // low byte from word

#define DUMMY 0xff
#define DUMMY_CHAR DUMMY

// Tokens (necessary  because at NPO/IDLE (and CS active) only 0xff is on the data/command line)
#define SDMMC_START_DATA_BLOCK_TOKEN          0xfe   // Data token start byte, Start Single Block Read
#define SDMMC_START_DATA_MULTIPLE_BLOCK_READ  0xfe   // Data token start byte, Start Multiple Block Read
#define SDMMC_START_DATA_BLOCK_WRITE          0xfe   // Data token start byte, Start Single Block Write
#define SDMMC_START_DATA_MULTIPLE_BLOCK_WRITE 0xfc   // Data token start byte, Start Multiple Block Write
#define SDMMC_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xfd   // Data toke stop byte, Stop Multiple Block Write

// an affirmative R1 response (no errors)
#define SDMMC_R1_RESPONSE       0x00

// this variable will be used to track the current block length
// this allows the block length to be set only when needed
// uint32_t _BlockLength = 0;

// error/success codes
#define SDMMC_SUCCESS           0x00
#define SDMMC_BLOCK_SET_ERROR   0x01
#define SDMMC_RESPONSE_ERROR    0x02
#define SDMMC_DATA_TOKEN_ERROR  0x03
#define SDMMC_INIT_ERROR        0x04
#define SDMMC_CRC_ERROR         0x10
#define SDMMC_WRITE_ERROR       0x11
#define SDMMC_OTHER_ERROR       0x12
#define SDMMC_TIMEOUT_ERROR     0xFF

// commands: first bit 0 (start bit), second 1 (transmission bit); CMD-number + 0ffsett 0x40
#define SDMMC_GO_IDLE_STATE          0x40     //CMD0
#define SDMMC_SEND_OP_COND           0x41     //CMD1
#define SDMMC_READ_CSD               0x49     //CMD9
#define SDMMC_SEND_CID               0x4a     //CMD10
#define SDMMC_STOP_TRANSMISSION      0x4c     //CMD12
#define SDMMC_SEND_STATUS            0x4d     //CMD13
#define SDMMC_SET_BLOCKLEN           0x50     //CMD16 Set block length for next read/write
#define SDMMC_READ_SINGLE_BLOCK      0x51     //CMD17 Read block from memory
#define SDMMC_READ_MULTIPLE_BLOCK    0x52     //CMD18
#define SDMMC_CMD_WRITEBLOCK         0x54     //CMD20 Write block to memory
#define SDMMC_WRITE_BLOCK            0x58     //CMD24
#define SDMMC_WRITE_MULTIPLE_BLOCK   0x59     //CMD25
#define SDMMC_WRITE_CSD              0x5b     //CMD27 PROGRAM_CSD
#define SDMMC_SET_WRITE_PROT         0x5c     //CMD28
#define SDMMC_CLR_WRITE_PROT         0x5d     //CMD29
#define SDMMC_SEND_WRITE_PROT        0x5e     //CMD30
#define SDMMC_TAG_SECTOR_START       0x60     //CMD32
#define SDMMC_TAG_SECTOR_END         0x61     //CMD33
#define SDMMC_UNTAG_SECTOR           0x62     //CMD34
#define SDMMC_TAG_EREASE_GROUP_START 0x63     //CMD35
#define SDMMC_TAG_EREASE_GROUP_END   0x64     //CMD36
#define SDMMC_UNTAG_EREASE_GROUP     0x65     //CMD37
#define SDMMC_EREASE                 0x66     //CMD38
#define SDMMC_READ_OCR               0x67     //CMD39
#define SDMMC_CRC_ON_OFF             0x68     //CMD40


//----------------------------------------------------------------------------
// SPI/UART port selections.  Select which port will be used for the interface
//----------------------------------------------------------------------------
//#define SPI_SER_INTF      SER_INTF_USART1  // Interface to MMC
#define SPI_SER_INTF      SER_INTF_BITBANG  // Interface to MMC

// SPI port definitions              // Adjust the values for the chosen
#define MMC_PxSEL         SPI_PxSEL      // interfaces, according to the pin
#define MMC_PxDIR         SPI_PxDIR      // assignments indicated in the
#define MMC_PxIN          SPI_PxIN       // chosen MSP430 device datasheet.
#define MMC_PxOUT         SPI_PxOUT
#define MMC_SIMO          SPI_SIMO
#define MMC_SOMI          SPI_SOMI
#define MMC_UCLK          SPI_UCLK

// Chip Select
#define MMC_CS_PxOUT      P5OUT
#define MMC_CS_PxDIR      P5DIR
#define MMC_CS            0x01

// Card Detect
#define MMC_CD_PxIN       P5IN
#define MMC_CD_PxDIR      P5DIR
#define MMC_CD            0x40

/*
// Chip Select
#define MMC_CS_PxOUT      P3OUT
#define MMC_CS_PxDIR      P3DIR
#define MMC_CS            0x10

// Card Detect
#define MMC_CD_PxIN       P3IN
#define MMC_CD_PxDIR      P3DIR
#define MMC_CD            0x00
*/

#define CS_LOW()    SDMMC_CS_PxOUT &= ~SDMMC_CS               // Card Select
#define CS_HIGH()   while(!halSPITXDONE); SDMMC_CS_PxOUT |= SDMMC_CS  // Card Deselect


// sdmmc init
uint8_t sdmmcInit(void);

// check if MMC card is present
uint8_t sdmmcPing(void);

// send command to MMC
void sdmmcSendCmd (const uint8_t cmd, uint32_t data, const uint8_t crc);

// set MMC in Idle mode
uint8_t sdmmcGoIdle();

// Check if sdmmc card is still busy
uint8_t sdmmcCheckBusy(void);

// set MMC block length of count=2^n Byte
uint8_t sdmmcSetBlockLength (const uint32_t);

// read a size Byte big block beginning at the address.
uint8_t sdmmcReadBlock(const uint32_t address, const uint32_t count, uint8_t *pBuffer);
#define sdmmcReadSector(sector, pBuffer) sdmmcReadBlock(sector*512ul, 512, pBuffer)

// write a 512 Byte big block beginning at the (aligned) address
uint8_t sdmmcWriteBlock (const uint32_t address, const uint32_t count, uint8_t *pBuffer);
#define sdmmcWriteSector(sector, pBuffer) sdmmcWriteBlock(sector*512ul, 512, pBuffer)

// Read Register arg1 with Length arg2 (into the buffer)
uint8_t sdmmcReadRegister(const uint8_t, const uint8_t, uint8_t *pBuffer);

// Read the Card Size from the CSD Register
uint32_t sdmmcReadCardSize(void);


#endif /* PFAL_SDMMC_SDMMC_H_ */
