#include "IIC.h"
/*********************************************************************
**  STM32模拟IIC通信
**  文件编码： GBK2312
**********************************************************************/

 __IO unsigned int timer2Delay;

/*****************************
**  初始化IIC的IO口
**
******************************/
void IIC_IOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE );  //使能IO口时钟
    
    GPIO_InitStructure.GPIO_Pin = IIC_SCL_IO | IIC_SDA_IO;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;       //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_SetBits(GPIOB,IIC_SCL_IO | IIC_SDA_IO);            //置位SDA，SCL    
}

/*****************************
**  起始信号
**
******************************/
void IIC_Start(void)
{
    SDA_OUT(); //sda 线输出
    IIC_SDA=1;
    IIC_SCL=1;
    IIC_DELAY(4);
    
    IIC_SDA=0;//当时钟线为高时，SDA拉低则起始信号才有效
    IIC_DELAY(4);
    
    IIC_SCL=0;//钳住 I2C 总线，准备发送或接收数据
}

/*****************************
**  停止信号
**
******************************/
void IIC_Stop(void)
{
    SDA_OUT();//sda 线输出
    IIC_SCL=0;
    IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
    IIC_DELAY(4);
    
    IIC_SCL=1;
    IIC_DELAY(1);
    IIC_SDA=1;//发送 I2C 总线结束信号
    IIC_DELAY(4);
}

/*****************************
**  等待应答信号
**  返回值： 1-接收应答失败， 0-为接收成功
******************************/
unsigned char IIC_Wait_Ack(void)
{
    unsigned char ucErrTime=0;
    SDA_IN(); //SDA 设置为输入
    IIC_SDA=1;IIC_DELAY(1);
    IIC_SCL=1;IIC_DELAY(1);
    
    while(READ_SDA)       //判断SDA上是否有低电平发生
    {
        ucErrTime++;
        if(ucErrTime>250)   //如果在一段时间内没有接收到应答信号，则发送停止信号
        {
            IIC_Stop();
            return 1;
        }
    }
    
    IIC_SCL=0;//时钟输出 0
    return 0;
}

/*****************************
**  产生应答信号
**  
******************************/
void IIC_Ack(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=0;
    IIC_DELAY(2);   //数据改变有效
    
    IIC_SCL=1;
    IIC_DELAY(2);
    IIC_SCL=0;
}

/*****************************
**  不产生应答信号
**  
******************************/
void IIC_NoAck(void)
{
    IIC_SCL=0;
    SDA_OUT();
    IIC_SDA=1;
    IIC_DELAY(2);
    
    IIC_SCL=1;
    IIC_DELAY(2);
    IIC_SCL=0;
}

/*****************************
**  发送一个字节的数据
**  
******************************/
void IIC_Send_Byte(unsigned char txd)
{
    unsigned char t;
    SDA_OUT();
    
    IIC_SCL=0;//拉低时钟开始数据传输，时SDA上的数据改变有效
    for(t=0;t<8;t++)
    {
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1;
        IIC_DELAY(2); //对 TEA5767 这三个延时都是必须的
        
        IIC_SCL=1;
        IIC_DELAY(2); //保持数据一段时间
        
        IIC_SCL=0;
        IIC_DELAY(2);
    }
}

/*****************************
**  读一个字节的数据
**  形式参数：ack=1，发送ACK， ack=0，发送NACK
******************************/
unsigned char IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();//SDA 设置为输入
    
    for(i=0;i<8;i++ )
    {
        IIC_SCL=0;
        IIC_DELAY(2);
        IIC_SCL=1;
        receive<<=1;
        
        if(READ_SDA)receive++;
        IIC_DELAY(1);
    }
    if (!ack)
        IIC_NoAck();//发送 nACK
    else
        IIC_Ack(); //发送 ACK
    
    return receive;
}

/*******************************************************************
**  基本延时函数1ms时间基础 
**  TIM2定时器1ms，中断
**  
**  
**  
********************************************************************/
void delay(unsigned int ims)
{
	timer2Delay = ims;
	while(timer2Delay!=0);
}
