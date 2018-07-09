
#ifndef TIMER_H
#define TIMER_H

	#include "stm32f10x.h"
	#include "data_type.h"
	
  extern __IO unsigned int  timer2Delay;
	
	void timer2Config(void);			//定时器配置函数：中断、IO引脚配置
	void TIM1_PWMConfig(void);		//高级定时器TIM1配置:IO口、定时器
	
#endif
