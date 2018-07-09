#include "stm32f10x_driver.h"
#include "led.h"
#include "motor.h"
#include "oled.h"

__IO unsigned int camera_power_time = 0;
__IO unsigned int camera_tri_time = 0;

/***********************************************
	系统初始化函数
************************************************/
void BSPInit(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); /* 0-15抢占优先级别，次优先级为0，无效 */
//	usart1_config();
	usart2_config();
	usart3_config();
	LED_IO_Config();
	outputPinConfig();
	extiLineConfig();
	timer2Config();
	IWDG_Init(3,3000);  //4*2^3 *1000 / 40 = 800ms
}

/************************************************
*	
*
*************************************************/
void RCC_Config(void)
{
	
}

/************************************************
	输出IO口的配置
*************************************************/
void outputPinConfig(void)
{
	GPIO_InitTypeDef GPIOInitStruct;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );
	
	//GPIOB
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOInitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIOInitStruct);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
}

/************************************************
	输入IO口的配置
*************************************************/
void inputPinConfig(void)
{
	GPIO_InitTypeDef GPIOInitStruct;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE );
	
	//GPIOC
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIOInitStruct);
	
	//GPIOD
	GPIOInitStruct.GPIO_Pin = GPIO_Pin_3;
	GPIOInitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIOInitStruct);
	
}

/*************************************************
	中断嵌套配置
**************************************************/
void extiLineConfig(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;		
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	
	/* config the extiline(PA0-1) clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	
	/* EXTI line gpio config() */	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	     //上拉输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);    
	
	/* EXTI line(PA0-1) mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5 | GPIO_PinSource6); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line5 | EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;   //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
  
	EXTI_ClearITPendingBit(EXTI_Line5);
	EXTI_ClearITPendingBit(EXTI_Line6);
}

/*************************************************************************
		外部中断0函数
*************************************************************************/
void EXTI0_IRQHandler(void)
{
	static unsigned char status = 0;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  
	if(EXTI_GetFlagStatus(EXTI_Line0)==SET)
	{
		EXTI_ClearFlag(EXTI_Line0);
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

/*************************************************************************
		外部中断1函数
*************************************************************************/
void EXTI1_IRQHandler(void)
{  
	if(EXTI_GetFlagStatus(EXTI_Line1)==SET)
	{
		EXTI_ClearFlag(EXTI_Line1);
		EXTI_ClearITPendingBit(EXTI_Line1);
	}
}

/*************************************************************************
		中断函数
*************************************************************************/
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetFlagStatus(EXTI_Line3)==SET)
	{
//		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//		
//		/* The event has occurred, use the semaphore to unblock the task so the task
//		can process the event. */
//		xSemaphoreGiveFromISR( xSemaphore_key, &xHigherPriorityTaskWoken );  //获取信号量
//		
//		/* Clear the interrupt here. */
//		//KEY3DOWM = TRUE;
		EXTI_ClearFlag(EXTI_Line3);
		EXTI_ClearITPendingBit(EXTI_Line3);
//		
//		/* Now the task has been unblocked a context switch should be performed if
//		xHigherPriorityTaskWoken is equal to pdTRUE. NOTE: The syntax required to perform
//		a context switch from an ISR varies from port to port, and from compiler to
//		compiler. Check the web documentation and examples for the port being used to
//		find the syntax required for your application. */
//		IWDG_Feed();  //喂狗
//		portYIELD_FROM_ISR( xHigherPriorityTaskWoken );		
	}
}

void EXTI9_5_IRQHandler(void)
{  
	if(EXTI_GetFlagStatus(EXTI_Line5)==SET)
	{
		/* 开关机控制，先拉低再拉高 */
		PBout(12) = 0;
		camera_power_time = 80; /*800ms*/
		LED_Action(ON);
		
		EXTI_ClearFlag(EXTI_Line5);
		EXTI_ClearITPendingBit(EXTI_Line5);
	}
	
	if(EXTI_GetFlagStatus(EXTI_Line6)==SET)
	{
		/* 控制拍照，先拉低再拉高 */		
		PBout(13) = 0;
		PBout(14) = 0;;
		camera_tri_time = 80; /* 600ms */
		LED_Action(ON);
		
		EXTI_ClearFlag(EXTI_Line6);
		EXTI_ClearITPendingBit(EXTI_Line6);
	}
}

void EXTI15_10_IRQHandler(void)
{
	return;
}

//初始化独立看门狗  
//prer:分频数:0~7(只有低 3 位有效!)  
//分频因子=4*2^prer.但最大值只能是 256!  
//rlr:重装载寄存器值:低 11 位有效.  
//时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms).  
void IWDG_Init(u8 prer,u16 rlr)  
{  
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //①使能对寄存器 I 写操作  
	IWDG_SetPrescaler(prer);  //②设置 IWDG 预分频值:设置 IWDG 预分频值  
	IWDG_SetReload(rlr);  //②设置 IWDG 重装载值  
	IWDG_ReloadCounter(); //③按照 IWDG 重装载寄存器的值重装载 IWDG 计数器  
	IWDG_Enable();  //④使能 IWDG  
	RCC_LSICmd(ENABLE);//打开LSI

}  
//喂独立看门狗  
void IWDG_Feed(void)  
{  
	IWDG_ReloadCounter();//reload   
}  
