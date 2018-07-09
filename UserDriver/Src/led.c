
#include "led.h"

unsigned int flashingTime = 1000;       //bink time ms

void setFlashingTime(unsigned int timeMs)
{
	flashingTime = timeMs;
}
unsigned int getFlashingTime(void)
{
	return flashingTime;
}

void LED_IO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	

	RCC_APB2PeriphClockCmd( LED_CLK_SOURCE, ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = LED_IO;        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;               
	GPIO_Init( LED_IO_CLASS, &GPIO_InitStructure );  
	
	LED_Action(OFF);		//πÿ±’LEDœ‘ æ	
}

