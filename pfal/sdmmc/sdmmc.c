/*
 * sdmmc.c
 *
 *  Created on: Jul 13, 2019
 *      Author: ppali
 */
#include <math.h>
#include "sdmmc.h"
#include "../spi/spi.h"

// Function Prototypes
uint8_t sdmmcGetResponse(void);
uint8_t sdmmcGetXXResponse(const uint8_t resp);
uint8_t sdmmcCheckBusy(void);
uint8_t sdmmcGoIdle();

// Varialbes
uint8_t sdmmc_buffer[512] = { 0 };               // Buffer for sdmmc i/o for data and registers


// Initialize sdmmc card
uint8_t sdmmcInit(void)
{
  //raise CS and MOSI for 80 clock cycles
  //SendByte(0xff) 10 times with CS high
  //RAISE CS
  int i;

  // Port x Function           Dir       On/Off
  //         sdmmcCS            Out       0 - Active 1 - none Active
  //         Dout               Out       0 - off    1 - On -> init in SPI_Init
  //         Din                Inp       0 - off    1 - On -> init in SPI_Init
  //         Clk                Out       -                 -> init in SPI_Init
  //         sdmmcCD            In        0 - card inserted

  // Init SPI Module
  initSpi();

  // Chip Select
  SDMMC_CS_PxDIR |= SDMMC_CS;
  SDMMC_CS_PxOUT |= SDMMC_CS;

  SDMMC_CD_PxDIR &=  ~SDMMC_CD;

  //initialization sequence on PowerUp
  CS_HIGH();
  for(i=0;i<=9;i++)
    spiSendByte(DUMMY_CHAR);

  return (sdmmcGoIdle());
}

// set sdmmc in Idle mode
uint8_t sdmmcGoIdle()
{
  uint8_t response=0x01;
  CS_LOW();

  //Send Command 0 to put sdmmc in SPI mode
  sdmmcSendCmd(SDMMC_GO_IDLE_STATE,0,0x95);

  //Now wait for READY RESPONSE
  if(sdmmcGetResponse()!=0x01)
    return SDMMC_INIT_ERROR;

  while(response==0x01)
  {
    CS_HIGH();
    spiSendByte(DUMMY_CHAR);
    CS_LOW();
    sdmmcSendCmd(SDMMC_SEND_OP_COND,0x00,0xff);
    response=sdmmcGetResponse();
  }
  CS_HIGH();
  spiSendByte(DUMMY_CHAR);
  return (SDMMC_SUCCESS);
}

// sdmmc Get Responce
uint8_t sdmmcGetResponse(void)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  int i=0;

  uint8_t response;

  while(i<=64)
  {
    response=spiSendByte(DUMMY_CHAR);
    if(response==0x00)break;
    if(response==0x01)break;
    i++;
  }
  return response;
}

uint8_t sdmmcGetXXResponse(const uint8_t resp)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  int i=0;

  uint8_t response;

  while(i<=1000)
  {
    response=spiSendByte(DUMMY_CHAR);
    if(response==resp)break;
    i++;
  }
  return response;
}

// Check if sdmmc card is still busy
uint8_t sdmmcCheckBusy(void)
{
  //Response comes 1-8bytes after command
  //the first bit will be a 0
  //followed by an error code
  //data will be 0xff until response
  int i=0;

  uint8_t response;
  uint8_t rvalue;
  while(i<=64)
  {
    response=spiSendByte(DUMMY_CHAR);
    response &= 0x1f;
    switch(response)
    {
      case 0x05: rvalue=SDMMC_SUCCESS;break;
      case 0x0b: return(SDMMC_CRC_ERROR);
      case 0x0d: return(SDMMC_WRITE_ERROR);
      default:
        rvalue = SDMMC_OTHER_ERROR;
        break;
    }
    if(rvalue==SDMMC_SUCCESS)break;
    i++;
  }
  i=0;
  do
  {
    response=spiSendByte(DUMMY_CHAR);
    i++;
  }while(response==0);
  return response;
}
// The card will respond with a standard response token followed by a data
// block suffixed with a 16 bit CRC.

// read a size Byte big block beginning at the address.
uint8_t sdmmcReadBlock(const uint32_t address, const uint32_t count, uint8_t *pBuffer)
{
  uint8_t rvalue = SDMMC_RESPONSE_ERROR;

  // Set the block length to read
  if (sdmmcSetBlockLength (count) == SDMMC_SUCCESS)   // block length could be set
  {
    // CS = LOW (on)
    CS_LOW ();
    // send read command SDMMC_READ_SINGLE_BLOCK=CMD17
    sdmmcSendCmd (SDMMC_READ_SINGLE_BLOCK, address, 0xFF);
    // Send 8 Clock pulses of delay, check if the MMC acknowledged the read block command
    // it will do this by sending an affirmative response
    // in the R1 format (0x00 is no errors)
    if (sdmmcGetResponse() == 0x00)
    {
      // now look for the data token to signify the start of
      // the data
      if (sdmmcGetXXResponse(SDMMC_START_DATA_BLOCK_TOKEN) == SDMMC_START_DATA_BLOCK_TOKEN)
      {
        // clock the actual data transfer and receive the bytes; spi_read automatically finds the Data Block
        getSpiData(pBuffer, count);
        // get CRC bytes (not really needed by us, but required by MMC)
        spiSendByte(DUMMY_CHAR);
        spiSendByte(DUMMY_CHAR);
        rvalue = SDMMC_SUCCESS;
      }
      else
      {
        // the data token was never received
        rvalue = SDMMC_DATA_TOKEN_ERROR;      // 3
      }
    }
    else
    {
      // the MMC never acknowledge the read command
      rvalue = SDMMC_RESPONSE_ERROR;          // 2
    }
  }
  else
  {
    rvalue = SDMMC_BLOCK_SET_ERROR;           // 1
  }
  CS_HIGH ();
  spiSendByte(DUMMY_CHAR);
  return rvalue;
}// sdmmc_read_block

//uint8_t sdmmcWriteBlock (const uint32_t address)
uint8_t sdmmcWriteBlock (const uint32_t address, const uint32_t count, uint8_t *pBuffer)
{
  uint8_t rvalue = SDMMC_RESPONSE_ERROR;         // SDMMC_SUCCESS;
  //  uint8_t c = 0x00;

  // Set the block length to read
  if (sdmmcSetBlockLength (count) == SDMMC_SUCCESS)   // block length could be set
  {
    // CS = LOW (on)
    CS_LOW ();
    // send write command
    sdmmcSendCmd (SDMMC_WRITE_BLOCK,address, 0xFF);

    // check if the MMC acknowledged the write block command
    // it will do this by sending an affirmative response
    // in the R1 format (0x00 is no errors)
    if (sdmmcGetXXResponse(SDMMC_R1_RESPONSE) == SDMMC_R1_RESPONSE)
    {
      spiSendByte(DUMMY_CHAR);
      // send the data token to signify the start of the data
      spiSendByte(0xfe);
      // clock the actual data transfer and transmitt the bytes

      sendSpiData(pBuffer, count);

      // put CRC bytes (not really needed by us, but required by MMC)
      spiSendByte(DUMMY_CHAR);
      spiSendByte(DUMMY_CHAR);
      // read the data response xxx0<status>1 : status 010: Data accected, status 101: Data
      //   rejected due to a crc error, status 110: Data rejected due to a Write error.
      sdmmcCheckBusy();
      rvalue = SDMMC_SUCCESS;
    }
    else
    {
      // the MMC never acknowledge the write command
      rvalue = SDMMC_RESPONSE_ERROR;   // 2
    }
  }
  else
  {
    rvalue = SDMMC_BLOCK_SET_ERROR;   // 1
  }
  // give the MMC the required clocks to finish up what ever it needs to do
  //  for (i = 0; i < 9; ++i)
  //    spiSendByte(0xff);

  CS_HIGH ();
  // Send 8 Clock pulses of delay.
  spiSendByte(DUMMY_CHAR);
  return rvalue;
} // sdmmc_write_block

// send command to MMC
void sdmmcSendCmd (const uint8_t cmd, uint32_t data, const uint8_t crc)
{
  uint8_t frame[6];
  uint8_t temp;
  int i;
  frame[0]=(cmd|0x40);
  for(i=3; i >= 0; i--){
    temp=(uint8_t)(data>>(8*i));
    frame[4-i]=(temp);
  }
  frame[5]=(crc);
  sendSpiData(frame, 6);
}

//--------------- set blocklength 2^n ------------------------------------------------------
uint8_t sdmmcSetBlockLength (const uint32_t blocklength)
{
  // CS = LOW (on)
  CS_LOW ();
  // Set the block length to read
  sdmmcSendCmd(SDMMC_SET_BLOCKLEN, blocklength, 0xFF);

  // get response from MMC - make sure that its 0x00 (R1 ok response format)
  if(sdmmcGetResponse()!=0x00)
  { sdmmcInit();
    sdmmcSendCmd(SDMMC_SET_BLOCKLEN, blocklength, 0xFF);
    sdmmcGetResponse();
  }

  CS_HIGH ();

  // Send 8 Clock pulses of delay.
  spiSendByte(DUMMY_CHAR);

  return SDMMC_SUCCESS;
} // Set block_length

// Reading the contents of the CSD and CID registers in SPI mode is a simple
// read-block transaction.
uint8_t sdmmcReadRegister (const uint8_t cmd_register, const uint8_t length, uint8_t *pBuffer)
{
  uint8_t uc = 0;
  uint8_t rvalue = SDMMC_TIMEOUT_ERROR;

  if (sdmmcSetBlockLength (length) == SDMMC_SUCCESS)
  {
    CS_LOW ();
    // CRC not used: 0xff as last byte
    sdmmcSendCmd(cmd_register, 0x000000, 0xff);

    // wait for response
    // in the R1 format (0x00 is no errors)
    if (sdmmcGetResponse() == 0x00)
    {
      if (sdmmcGetXXResponse(0xfe)== 0xfe)
        for (uc = 0; uc < length; uc++)
          pBuffer[uc] = spiSendByte(DUMMY_CHAR);  //sdmmc_buffer[uc] = spiSendByte(0xff);
      // get CRC bytes (not really needed by us, but required by MMC)
      spiSendByte(DUMMY_CHAR);
      spiSendByte(DUMMY_CHAR);
      rvalue = SDMMC_SUCCESS;
    }
    else
      rvalue = SDMMC_RESPONSE_ERROR;
    // CS = HIGH (off)
    CS_HIGH ();

    // Send 8 Clock pulses of delay.
    spiSendByte(DUMMY_CHAR);
  }
  CS_HIGH ();
  return rvalue;
} // sdmmc_read_register

uint32_t sdmmcReadCardSize(void)
{
  // Read contents of Card Specific Data (CSD)

  uint32_t SDMMC_CardSize;
  uint8_t i,      // index
                 j,      // index
                 b,      // temporary variable
                 response,   // MMC response to command
                 sdmmc_C_SIZE;

  uint8_t sdmmc_READ_BL_LEN,  // Read block length
                sdmmc_C_SIZE_MULT;

  CS_LOW ();

  spiSendByte(SDMMC_READ_CSD);   // CMD 9
  for(i=4; i>0; i--)      // Send four dummy bytes
    spiSendByte(0);
  spiSendByte(DUMMY_CHAR);   // Send CRC byte

  response = sdmmcGetResponse();

  // data transmission always starts with 0xFE
  b = spiSendByte(DUMMY_CHAR);

  if( !response )
  {
    while (b != 0xFE) b = spiSendByte(DUMMY_CHAR);
    // bits 127:87
    for(j=5; j>0; j--)          // Host must keep the clock running for at
      b = spiSendByte(DUMMY_CHAR);

    // 4 bits of READ_BL_LEN
    // bits 84:80
    b =spiSendByte(DUMMY_CHAR);  // lower 4 bits of CCC and
    sdmmc_READ_BL_LEN = b & 0x0F;
    b = spiSendByte(DUMMY_CHAR);
    // bits 73:62  C_Size
    // xxCC CCCC CCCC CC
    sdmmc_C_SIZE = (b & 0x03) << 10;
    b = spiSendByte(DUMMY_CHAR);
    sdmmc_C_SIZE += b << 2;
    b = spiSendByte(DUMMY_CHAR);
    sdmmc_C_SIZE += b >> 6;
    // bits 55:53
    b = spiSendByte(DUMMY_CHAR);
    // bits 49:47
    sdmmc_C_SIZE_MULT = (b & 0x03) << 1;
    b = spiSendByte(DUMMY_CHAR);
    sdmmc_C_SIZE_MULT += b >> 7;
    // bits 41:37
    b = spiSendByte(DUMMY_CHAR);
    b = spiSendByte(DUMMY_CHAR);
    b = spiSendByte(DUMMY_CHAR);
    b = spiSendByte(DUMMY_CHAR);
    b = spiSendByte(DUMMY_CHAR);
  }

  for(j=4; j>0; j--)          // Host must keep the clock running for at
    b = spiSendByte(DUMMY_CHAR);  // least Ncr (max = 4 bytes) cycles after
                               // the card response is received
  b = spiSendByte(DUMMY_CHAR);
  CS_LOW ();

  SDMMC_CardSize = (sdmmc_C_SIZE + 1);
  // power function with base 2 is better with a loop
  // i = (pow(2,sdmmc_C_SIZE_MULT+2)+0.5);
  for(i = 2,j=sdmmc_C_SIZE_MULT+2; j>1; j--)
    i <<= 1;
  SDMMC_CardSize *= i;
  // power function with base 2 is better with a loop
  //i = (pow(2,sdmmc_READ_BL_LEN)+0.5);
  for(i = 2,j=sdmmc_READ_BL_LEN; j>1; j--)
    i <<= 1;
  SDMMC_CardSize *= i;

  return (SDMMC_CardSize);

}

uint8_t sdmmcPing(void)
{
  if (!(SDMMC_CD_PxIN & SDMMC_CD))
    return (SDMMC_SUCCESS);
  else
    return (SDMMC_INIT_ERROR);
}




