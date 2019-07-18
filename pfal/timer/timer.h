/*
 * timer.h
 *
 *  Created on: Jun 19, 2019
 *      Author: ppali
 */

#ifndef TIMER_H
#define TIMER_H

#include <msp430g2553.h>
#include "../inc_16/std_types.h"

void initTimer(void);
//int getTimerMilliSec(void);
//int getTimerMicroSec(void);
//int getTimerSec(void);

void initTimerPwm(void);

#endif  /* #ifndef TIMER_H */
