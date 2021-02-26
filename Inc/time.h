#ifndef _TIME_H
#define _TIME_H

#include "stm32f0xx.h"


typedef struct
{
	uint32_t TimeStart;
	uint32_t TimeInter;

}tsTimeType;


void SetTime(tsTimeType *TimeType,uint32_t TimeInter);
uint8_t  CompareTime(tsTimeType *TimeType);
#endif

