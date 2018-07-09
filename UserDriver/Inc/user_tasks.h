
#ifndef USER_TASKS_H
#define USER_TASKS_H
	
	#include "stm32f10x.h"
	#include "includes.h"
	#include "Data_Type.h"
	#include "FreeRTOS.h"
	#include "task.h"
	#include "usart.h"
	#include "led.h"
	#include "timers.h"
	
	#define	LED_TASK	"LED_TASK"
	#define USART1_TASK_NAME	"USART1_REC"
	#define USART2_TASK_NAME	"USART2_REC"
	#define USART3_TASK_NAME "USART3_REC"
	#define FEED_DOG_TASK_NAME 	"FEED_DOG"
 				
	/*** 队列句柄、长度 ***/
	void usart3_receive_task(void *pvParameters);
	void usart2_receive_task(void *pvParameters);
	void usart1_receive_task(void *pvParameters);
	
	void feed_dog_task(void *pvParameters);
	void LED_Task(void *pvParameters);
	
	void AppStart(void);
	void timerCallBack(TimerHandle_t xTimer);
	
	void request_send_data(void);
	
#endif
