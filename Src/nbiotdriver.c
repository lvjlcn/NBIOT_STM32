#include "nbiotdriver.h"
#include "usart.h"
#include "time.h"
#include "led.h"
#include <string.h>
static uint8_t CurrentRty;
static teATCmdNum ATRecCmdNum;
static teATCmdNum ATCurrentCmdNum;
static teATCmdNum ATNextCmdNum;
static teNB_TaskStatus NB_TaskStatus;
static tsTimeType TimeNB;
static tsTimeType TimeNBSendData;
uint8_t socket;
char NbSendData[100];

uchar remoteServer[20] = "10.10.10.10";  //change to you server ip
ushort remotePort = 10000;   //change to you server port
uchar senddata[100];
uchar sendlen;

tsATCmds ATCmds[] = 
{
    {"AT+NRB\r\n","Verified",10000,NO_REC,3},   //reboot
	{"AT+CGSN=1\r\n","OK",2000,NO_REC,3},   //Request Product Serial Number
	{"AT+CSQ\r\n","OK",3000,NO_REC,20},   // get signal strength
	{"AT+CGPADDR\r\n","+CGPADDR:1",2000,NO_REC,10},  //  get device ip address
	{"AT+NSOCR=STREAM,6,10000\r\n","OK",2000,NO_REC,1},  //create tcp socket,local port 
	{"AT+NSOCO=1,","OK",2000,NO_REC,0}, //connect server 
	{"AT+NSOSD=1,","OK",2000,NO_REC,0}, //send data， 
	{"AT+NSOCL=1\r\n","OK",2000,NO_REC,0}, //close socket
};


void ATSend(teATCmdNum ATCmdNum)
{
	//清空接收缓存区
	memset(Usart2type.Usart2RecBuff,0,USART2_REC_SIZE);
	ATCmds[ATCmdNum].ATStatus = NO_REC;
	
	ATRecCmdNum = ATCmdNum;
    if(ATCmdNum == AT_NSOCO) // for conncet server
    {
		memset(NbSendData,0,100);
		
		sprintf(NbSendData,"%s%s,%d\r\n",ATCmds[ATCmdNum].ATSendStr,remoteServer,remotePort);
//		printf("id=%d,nmgs=%s\n",ATCmdNum,NbSendData);
		HAL_UART_Transmit(&huart2,(uint8_t*)NbSendData,strlen(NbSendData),100);
		HAL_UART_Transmit(&huart1,(uint8_t*)NbSendData,strlen(NbSendData),100);
    }
	else if(ATCmdNum == AT_NSOSD)
	{
	
		memset(NbSendData,0,100);
		
		sprintf(NbSendData,"%s%d,%x%x%x\r\n",ATCmds[ATCmdNum].ATSendStr,sendlen,senddata[0],senddata[1],senddata[2]);
//		printf("id=%d,nmgs=%s\n",ATCmdNum,NbSendData);
		HAL_UART_Transmit(&huart2,(uint8_t*)NbSendData,strlen(NbSendData),100);
		HAL_UART_Transmit(&huart1,(uint8_t*)NbSendData,strlen(NbSendData),100);
	
	
	}
	else
	{
//	    printf("id %d,snd=%s\n",ATCmdNum,ATCmds[ATCmdNum].ATSendStr);
		HAL_UART_Transmit(&huart2,(uint8_t*)ATCmds[ATCmdNum].ATSendStr,strlen(ATCmds[ATCmdNum].ATSendStr),100);
		HAL_UART_Transmit(&huart1,(uint8_t*)ATCmds[ATCmdNum].ATSendStr,strlen(ATCmds[ATCmdNum].ATSendStr),100);
	}	
	//打开超时定时器
	SetTime(&TimeNB,ATCmds[ATCmdNum].TimeOut);
	
	
	
	//打开发送指示灯
	SetLedRun(LED_TX);
}

void ATRec(void)
{       
        uchar rssi;
        uchar ber;
        uchar a=255,b=255,c=255,d=255;
        uchar cid;
        uchar *startP;
		if(Usart2type.Usart2RecFlag)
		{
		    if(ATRecCmdNum ==AT_CSQ){
		        startP = strstr((const char*)Usart2type.Usart2RecBuff,"+CSQ");
                if(startP!= NULL)
                {
        		    sscanf(startP,"+CSQ: %d,%d",&rssi,&ber);
        		    if(rssi>0 && rssi<=31 )
        		    {
        		        printf("signalOK\n");
                        ATCmds[ATRecCmdNum].ATStatus = SUCCESS_REC;
        		    }else if(rssi ==99 || ber == 99){
                        ATCmds[ATRecCmdNum].ATStatus = ERROR_REC;
        		    }
                }
		    }else if(ATRecCmdNum ==AT_CGPADDR){
		        startP = strstr((const char*)Usart2type.Usart2RecBuff,"+CGPADDR");
                if(startP!= NULL)
                {
        		    sscanf(startP,"+CGPADDR: %d,%d.%d.%d.%d",&cid,&a,&b,&c,&d);
        		    if(a>0 && a<254 && b>=0 && b<=254 && c>=0 && c<=254 && d>=0 && d<=254){
        		        printf("get addr success\n");
                        ATCmds[ATRecCmdNum].ATStatus = SUCCESS_REC;
        		    }else{
                        ATCmds[ATRecCmdNum].ATStatus = ERROR_REC;
        		    }
                }
		    }
		    else if(ATRecCmdNum ==AT_NSOCR){
		        startP = strstr((const char*)Usart2type.Usart2RecBuff,"OK");
                if(startP!= NULL)
                {
					sscanf(startP,"%d\r\n\r\nOK",&socket);
                    ATCmds[ATRecCmdNum].ATStatus = SUCCESS_REC;
        		  
                }
		    }
		    else 
			{
			    startP = strstr((const char*)Usart2type.Usart2RecBuff,ATCmds[ATRecCmdNum].ATRecStr);
                if(startP!= NULL){
				    ATCmds[ATRecCmdNum].ATStatus = SUCCESS_REC;
				}

			}

			HAL_Delay(800);
			SetLedRun(LED_RX);
			HAL_UART_Transmit(&huart1,Usart2type.Usart2RecBuff,Usart2type.Usart2RecLen,100);
			Usart2type.Usart2RecFlag = 0;
			Usart2type.Usart2RecLen = 0;
		
		}

}





void NB_Init(void)
{

	NB_TaskStatus = NB_SEND;
	ATCurrentCmdNum = AT_NRB;
	ATNextCmdNum = AT_CGSN;

}


void NB_Task(void)
{

	while(1)
	{
	
		switch(NB_TaskStatus)
		{
			case NB_IDIE:
				return;
			case NB_SEND:
						if(ATCurrentCmdNum != ATNextCmdNum)
						{
							CurrentRty = ATCmds[ATCurrentCmdNum].RtyNum;
						}
						ATSend(ATCurrentCmdNum);
						NB_TaskStatus = NB_WAIT;
				return;
			case NB_WAIT:
					ATRec();
					if(ATCmds[ATCurrentCmdNum].ATStatus == SUCCESS_REC)
					{
					
						if(ATCurrentCmdNum == AT_NSOCL)
						{
							NB_TaskStatus = NB_IDIE;
							break;
						}
						else
						{
							
							ATCurrentCmdNum += 1;
							ATNextCmdNum = ATCurrentCmdNum+1;
							NB_TaskStatus = NB_SEND;
							break;

						}
					}
					else if(CompareTime(&TimeNB))
					{
						ATCmds[ATCurrentCmdNum].ATStatus = TIME_OUT;
						if(CurrentRty > 0)
						{
							CurrentRty--;
							ATNextCmdNum = ATCurrentCmdNum;
							NB_TaskStatus = NB_SEND;
							break;
						}
						else
						{
							NB_Init();
							return;
						}
				
					}
				return;
			case NB_ACCESS:
						break;
			default:
				return;

		}
	
	}



}


