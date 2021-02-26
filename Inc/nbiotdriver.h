#ifndef _NBIOTDRIVER_H
#define _NBIOTDRIVER_H

#include "stm32f0xx.h"


typedef enum
{
	SUCCESS_REC = 0,
	TIME_OUT,
	NO_REC,
	ERROR_REC

}teATStatus;

typedef enum
{
	NB_IDIE = 0,
	NB_SEND,
	NB_WAIT,
	NB_ACCESS
}teNB_TaskStatus;
typedef enum
{
    AT_NRB=0,
	AT_CGSN,
	AT_CSQ,
	AT_CGPADDR,
    AT_NSOCR,
    AT_NSOCO,
    AT_NSOSD,
    AT_NSOCL,
	AT_IDIE,

}teATCmdNum;
typedef struct
{
	char *ATSendStr;
	char *ATRecStr;
	uint16_t TimeOut;
	teATStatus ATStatus;
	uint8_t RtyNum;
}tsATCmds;

#define uchar unsigned char
#define uint unsigned int
#define ushort unsigned short

void NB_Init(void);


void NB_Task(void);

#endif
