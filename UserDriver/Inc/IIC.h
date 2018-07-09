#ifndef _IIC_
#define _IIC_

#include "stm32f10x.h"
#include "Data_Type.h"


/*	配置IIC的SDA、SCL两个端口 */
#define	IIC_SCL_IO 	 GPIO_Pin_6
#define IIC_SDA_IO   GPIO_Pin_7

/*  SDA端口动态改变输入输出状态定义 */
#define SDA_IN() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0X80000000;}    //配置上拉输入
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0X30000000;}   //配置推挽输出

/* IIC 操作定义 */
#define IIC_SDA 	PBout(7)
#define READ_SDA	PBin(7)
#define IIC_SCL     PBout(6)
#define IIC_DELAY(time)	 delay(time)

/*	定义的函数原型	*/
void delay(unsigned int ims);
void IIC_IOInit(void);
void IIC_Start(void);
void IIC_Stop(void);
unsigned char IIC_Wait_Ack(void);
void IIC_NoAck(void);
void IIC_Ack(void);
void IIC_Send_Byte(unsigned char txd);
unsigned char IIC_Read_Byte(unsigned char ack);

#endif

