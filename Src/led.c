#include "led.h"
#include "time.h"

const uint16_t LedPins[LED_NUMBER] = 
{

	GPIO_PIN_0,
	GPIO_PIN_1,
	GPIO_PIN_2

};


static tsTimeType TimeLeds[LED_NUMBER];

static teLedTaskStatus LedTaskStatus[LED_NUMBER];
void LedOn(teLedNums LedNums)
{

	
	HAL_GPIO_WritePin(GPIOB,LedPins[LedNums],GPIO_PIN_RESET);

}

void LedOff(teLedNums LedNums)
{

	
	HAL_GPIO_WritePin(GPIOB,LedPins[LedNums],GPIO_PIN_SET);

}


void LedInit(void)
{
	int i;
	for(i = 0;i < LED_NUMBER;i++)
	{
	
	
		LedOff(i);
	
	
	}
	

}

void SetLedRun(teLedNums LedNums)
{

		LedTaskStatus[LedNums] = LED_RUN;
		
		

}


void LedTask(void)
{
	int i;
	
	for(i = 0;i < LED_NUMBER;i++)
	{
	
		if(LedTaskStatus[i] == LED_RUN)
		{
		
			LedOn(i);
			SetTime(&TimeLeds[i],100);
			LedTaskStatus[i] = LED_DELAY;
		
		}
		else if(LedTaskStatus[i] == LED_DELAY)
		{	
		
		
			if(CompareTime(&TimeLeds[i]))
			{
			
				LedOff(i);
				LedTaskStatus[i] = LED_STOP;
			
			
			}

		}

	}
	
	

}
