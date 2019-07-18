/*
 * i2c.h
 *
 *  Created on: Jun 19, 2019
 *      Author: ppali
 *                   MSP430G2553         3.3V
 *                 -----------------   /|\ /|\
 *            /|\ |                 |   |  4.7k
 *             |  |                 |  4.7k |
 *             ---|RST              |   |   |
 *                |                 |   |   |
 *                |             P1.6|---|---+- I2C Clock (UCB0SCL)
 *                |                 |   |
 *                |             P1.7|---+----- I2C Data (UCB0SDA)
 *                |                 |
 *
 *
 */


//******************************************************************************
// Example Commands ************************************************************
//******************************************************************************

#define SLAVE_ADDR  0x48

/* CMD_TYPE_X_SLAVE are example commands the master sends to the slave.
 * The slave will send example SlaveTypeX buffers in response.
 *
 * CMD_TYPE_X_MASTER are example commands the master sends to the slave.
 * The slave will initialize itself to receive MasterTypeX example buffers.
 * */

#define CMD_TYPE_0_SLAVE      0
#define CMD_TYPE_1_SLAVE      1
#define CMD_TYPE_2_SLAVE      2

#define CMD_TYPE_0_MASTER      3
#define CMD_TYPE_1_MASTER      4
#define CMD_TYPE_2_MASTER      5

#define TYPE_0_LENGTH   1
#define TYPE_1_LENGTH   2
#define TYPE_2_LENGTH   6

#define MAX_BUFFER_SIZE     20

/* MasterTypeX are example buffers initialized in the master, they will be
 * sent by the master to the slave.
 * SlaveTypeX are example buffers initialized in the slave, they will be
 * sent by the slave to the master.
 * */

unsigned char MasterType2 [TYPE_2_LENGTH] = {'F', '4', '1', '9', '2', 'B'};
unsigned char MasterType1 [TYPE_1_LENGTH] = { 8, 9};
unsigned char MasterType0 [TYPE_0_LENGTH] = { 11};


unsigned char SlaveType2 [TYPE_2_LENGTH] = {0};
unsigned char SlaveType1 [TYPE_1_LENGTH] = {0};
unsigned char SlaveType0 [TYPE_0_LENGTH] = {0};

//******************************************************************************
// General I2C State Machine ***************************************************
//******************************************************************************


typedef enum I2C_ModeEnum{
    IDLE_MODE,
    NACK_MODE,
    TX_REG_ADDRESS_MODE,
    RX_REG_ADDRESS_MODE,
    TX_DATA_MODE,
    RX_DATA_MODE,
    SWITCH_TO_RX_MODE,
    SWITHC_TO_TX_MODE,
    TIMEOUT_MODE
} I2C_Mode;


void initI2c(void);

void CopyArray(unsigned char *source, unsigned char *dest, unsigned char count);
I2C_Mode sendI2cData(unsigned char slave_addr, unsigned char *d, unsigned char num_bytes);
I2C_Mode getI2cData(unsigned char slave_addr, unsigned char *d, unsigned char num_bytes);

