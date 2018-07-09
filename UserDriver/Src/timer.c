
#include "timer.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "usart.h"
#include "task.h"
#include "stm32f10x_driver.h"

extern com_rx_buff com1_rx_buff;
extern com_rx_buff com3_rx_buff;
extern xQueueHandle com1_tx_queue_handle;
extern xQueueHandle com1_rx_queue_handle;
extern xQueueHandle com3_rx_queue_handle;
extern xQueueHandle com3_tx_queue_handle;

__IO unsigned int  timer2Delay = 0;

/*****************************************************
*  定时器配置： IO、中断、定时器基本配置
*
******************************************************/

void delay_ms(unsigned int timeMs)
{
	timer2Delay = timeMs/10;
	while(timer2Delay);
}
	 
void timer2Config(void)
{
	NVIC_InitTypeDef nvicStruct;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* TIM2 clock enable : 72MHz*/
	TIM_DeInit(TIM2);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 
	TIM_InternalClockConfig(TIM2);


	/* Time base configuration : 1000Hz = 72000000/(399+1)*(719+1)*/	 
	TIM_TimeBaseStructure.TIM_Period = 100-1;       
	TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;	 //10ms 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM2, ENABLE);			 					
	TIM_Cmd(TIM2, ENABLE); 

	nvicStruct.NVIC_IRQChannel = TIM2_IRQn;
	nvicStruct.NVIC_IRQChannelPreemptionPriority = 10;
	nvicStruct.NVIC_IRQChannelSubPriority = 0;
	nvicStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStruct); 

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);		//打开TIM2定时器中断
}

/*****************************************************
*  定时器1 PWM 配置函数
*
******************************************************/
void TIM1_PWMConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
   TIM_OCInitTypeDef  TIM_OCInitStructure;
	u16 CCR1_Val = 1000;        
	u16 CCR2_Val = 18888;
	u16 CCR3_Val = 2500;
	u16 CCR4_Val = 1250;
	/* ----------------------------------------------------------------------- 
    TIM3 Channel1 duty cycle = (TIM3_CCR1/ TIM3_ARR+1)* 100% = 50%
    TIM3 Channel2 duty cycle = (TIM3_CCR2/ TIM3_ARR+1)* 100% = 37.5%
    TIM3 Channel3 duty cycle = (TIM3_CCR3/ TIM3_ARR+1)* 100% = 25%
    TIM3 Channel4 duty cycle = (TIM3_CCR4/ TIM3_ARR+1)* 100% = 12.5%
  ----------------------------------------------------------------------- */ 
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	
	/*** TIM 输出通道IO引脚配置 ***/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;  //PA8-PA11 -> TIM1_CH1-TIM1-CH4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 			//复用推完输出       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
        
  TIM_TimeBaseStructure.TIM_Period = 20000-1;//自动重装载寄存器周期的值  2s
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)(7200-1);          
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;        
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	//向上计数
  TIM_TimeBaseStructure.TIM_RepetitionCounter=0;    
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 
  
//  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;		//脉宽调制模式1 CNT<CCR时输出为1，否则为0           
//  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  	//输出使能     
//  TIM_OCInitStructure.TIM_Pulse = CCR1_Val; 								//输入待比较寄存器的值         
//  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;			//输出比较极性为 高电平
//  TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  //TIM_OCInitStructure.TIM_Pulse = CCR1_Val;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  //TIM_OC1Init(TIM1, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = CCR2_Val;
  TIM_OC2Init(TIM1, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = CCR3_Val;
  TIM_OC3Init(TIM1, &TIM_OCInitStructure);

  TIM_OCInitStructure.TIM_Pulse = CCR4_Val;
  TIM_OC4Init(TIM1, &TIM_OCInitStructure);

  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);

  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);//高级定时器需要输出PWM需要调用该函数  
  }

/*****************************************************
*  定时器2中断入口函数
*
******************************************************/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		if(camera_power_time<=0)
		{
			PBout(12) = 1;
		}else
		{
			--camera_power_time;
		}
		if(camera_tri_time<=0)
		{
			PBout(13) = 1;
			PBout(14) = 1;
		}else
		{
			--camera_tri_time;
		}
	  TIM_ClearFlag(TIM2, TIM_IT_Update);		
	}			
}
