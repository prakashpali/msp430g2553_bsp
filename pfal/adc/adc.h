/*
 * temp.h
 *
 *  Created on: Jun 19, 2019
 *      Author: ppali
 */
#ifndef TEMP_H
#define TEMP_H

#include <msp430g2553.h>
#include "../inc_16/std_types.h"

void initTempSensor(void);
uint32_t getTemp(void);

#endif /* #ifndef TEMP_H */
