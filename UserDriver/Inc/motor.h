
#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"
#include "Data_Type.h"

#define  LEG_UP    0
#define  LEG_DOWN  1

//引脚位定义
#define LEFT_IN1_BIT PBout(6)
#define LEFT_IN2_BIT PBout(7)	//左边起落架控制引脚
#define LEFT_LEG_UP			LEFT_IN1_BIT=0; LEFT_IN2_BIT=1;			//上升
#define LEFT_LEG_DOWN   LEFT_IN1_BIT=1; LEFT_IN2_BIT=0;      //下降
#define LEFT_LEG_STOP   LEFT_IN1_BIT=LEFT_IN2_BIT=0;				//停止

#define RIGHT_IN1_BIT PBout(8)
#define RIGHT_IN2_BIT PBout(9) //右边起落架控制引脚
#define RIGHT_LEG_UP			RIGHT_IN1_BIT=0; RIGHT_IN2_BIT=1;  //上升
#define RIGHT_LEG_DOWN		RIGHT_IN1_BIT=1; RIGHT_IN2_BIT=0;  //下降
#define  RIGHT_LEG_STOP	  RIGHT_IN1_BIT=RIGHT_IN2_BIT=0;      //停止

#define LANDING_GEAR_UP	  { LEFT_LEG_UP; RIGHT_LEG_UP; }
#define LANDING_GEAR_DOWN	{ LEFT_LEG_DOWN; RIGHT_LEG_DOWN; }
#define LANDING_GEAR_STOP { LEFT_LEG_STOP; RIGHT_LEG_STOP; }


//左边起落架
//#define  LEFT_LEG_UP       GPIO_SetBits(GPIOA,GPIO_Pin_6);   GPIO_ResetBits(GPIOA,GPIO_Pin_7)
//#define  LEFT_LEG_DOWN     GPIO_SetBits(GPIOA,GPIO_Pin_7);   GPIO_ResetBits(GPIOA,GPIO_Pin_6)
//#define  LEFT_LEG_STOP     GPIO_ResetBits(GPIOA,GPIO_Pin_6); GPIO_ResetBits(GPIOA,GPIO_Pin_7)

////右边起落架
//#define  RIGHT_LEG_UP      GPIO_SetBits(GPIOB,GPIO_Pin_8);   GPIO_ResetBits(GPIOB,GPIO_Pin_9)
//#define  RIGHT_LEG_DOWN    GPIO_SetBits(GPIOB,GPIO_Pin_9);   GPIO_ResetBits(GPIOB,GPIO_Pin_8)
//#define  RIGHT_LEG_STOP    GPIO_ResetBits(GPIOB,GPIO_Pin_8); GPIO_ResetBits(GPIOB,GPIO_Pin_9)


void Motor_IO_Config(void);
void Undercarriage_MotorControl(volatile u16 *ticks, u16 n);


#endif /* __LED_H */
