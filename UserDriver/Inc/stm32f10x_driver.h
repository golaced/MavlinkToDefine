#ifndef STM32F10X_DRIVER_H
#define STM32F10X_DRIVER_H

	#include "stm32f10x.h"
	#include "Data_Type.h"
	#include "FreeRTOS.h"
  #include "semphr.h"
	#include "adc.h"
	#include "ili9341.h"
	#include "usart.h"
	#include "timer.h"
	#include "W25Qx.h"
	#include "SPI.h"
	
	/***	函数原型	***/
	void systemInit(void);
	void outputPinConfig(void);
	void inputPinConfig(void);
	void extiLineConfig(void);
	void IWDG_Init(u8 prer,u16 rlr);  
	void IWDG_Feed(void); 
  void BspInit(void); 

	
	/*** 中断函数原型 ***/
	void EXTI3_IRQHandler(void);
	void EXTI15_10_IRQHandler(void);
	
//	extern SemaphoreHandle_t xSemaphore_key;  //by file @user_tasks.h
//	extern UCHAR KEY3DOWM;
	extern SemaphoreHandle_t xSemaphore_landing_gear;  
//  extern SemaphoreHandle_t xSemaphore_usart3_rx;
//  extern SemaphoreHandle_t xSemaphore_usart1_rx; 	//by file @user_tasks.h  

__IO extern  unsigned int camera_power_time;
__IO extern  unsigned int camera_tri_time;

#endif
