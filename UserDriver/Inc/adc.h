
#ifndef ADC_H
#define ADC_H


	#include "stm32f10x.h"
	#include "math.h"
	
	#define V25_TYPE  1.43
	#define AVG_SLOPE 4.3
	
	static void ADC1_GPIO_Config(void);
	static void ADC1_Mode_Config(void);
	void ADC1_Init(void);
	float innerTempValue(void);
	
	float getCurrentValue(void);
	float getInnerVolValue(void);
	float getInnerTempValue(void);
	void floatToString(char *pc, float f, unsigned char intn, unsigned char decn);	


#endif
