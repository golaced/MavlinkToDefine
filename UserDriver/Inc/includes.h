

/*** STM32相关的文件 ***/

#include "stm32f10x.h"


/*** 用户相关的文件 ***/
#define	DELAY_10MS	1	  //系统时钟为9MHZ，中断时间为10ms

#include "Data_Type.h"
#include "stm32f10x_driver.h"
#include "user_tasks.h"


//static __IO uint32_t TimingDelay;
//void TimingDelay_Decrement(void);
//void Delay(__IO uint32_t nTime);

///*** 系统时钟延迟函数 ***/
//void TimingDelay_Decrement(void)
//{
//  if (TimingDelay != 0x00)
//  { 
//    TimingDelay--;
//  }
//}

///*** 利用系统时钟中断实现延迟函数 ***/
//void Delay(__IO uint32_t nTime)
//{ 
//  TimingDelay = nTime;

//  while(TimingDelay != 0);
//}


