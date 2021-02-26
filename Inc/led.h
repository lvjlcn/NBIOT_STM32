#ifndef _LED_H
#define _LED_H

#include "stm32f0xx.h"


#define LED_NUMBER 3

typedef enum
{
	LED_NET = 0,
	LED_RX,
	LED_TX


}teLedNums;

typedef enum
{
	LED_STOP = 0,
	LED_RUN,
	LED_DELAY
}teLedTaskStatus;


void LedOn(teLedNums LedNums);
void LedOff(teLedNums LedNums);
void LedInit(void);

void SetLedRun(teLedNums LedNums);


void LedTask(void);

#endif


