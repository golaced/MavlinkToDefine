#include "AT24CXX.H"

/*****************************
**  初始化AT24Cxx， IIC接口
**  
******************************/
void AT24CXX_Init(void)
{
    IIC_IOInit();
}

/*****************************
**  在 AT24CXX 指定地址读出一个数据
**  ReadAddr:开始读数的地址
**  返回值 :读到的数据
******************************/
unsigned char AT24CXX_ReadOneByte(unsigned short int ReadAddr)
{
    unsigned char temp=0;
    IIC_Start();
    if(EEPROM_TYPE>AT24C16)
    {
        IIC_Send_Byte(AT24CXX_WRITE); //发送写命令
        IIC_Wait_Ack();
        IIC_Send_Byte(ReadAddr>>8);//发送高地址 
    }
    else IIC_Send_Byte(AT24CXX_WRITE); //发送器件地址 0XA0,写数据
    IIC_Wait_Ack();
    
    IIC_Send_Byte(ReadAddr%256); //发送低地址
    IIC_Wait_Ack();
    
    IIC_Start();
    IIC_Send_Byte(AT24CXX_READ); //进入接收模式
    IIC_Wait_Ack();
    
    temp=IIC_Read_Byte(0);
    IIC_Stop();//产生一个停止条件
    return temp;
}

/*****************************
**  在 AT24CXX 指定地址写入一个数据
**  WriteAddr :写入数据的目的地址
**  DataToWrite:要写入的数据
******************************/
void AT24CXX_WriteOneByte(unsigned short int WriteAddr,unsigned char DataToWrite)
{
    IIC_Start();
    if(EEPROM_TYPE>AT24C16)
    {
        IIC_Send_Byte(AT24CXX_WRITE); //发送写命令
        IIC_Wait_Ack();
        IIC_Send_Byte(WriteAddr>>8);//发送高地址
    }else 
        IIC_Send_Byte(AT24CXX_WRITE); //发送器件地址 0XA0,写数据
    
    IIC_Wait_Ack();
    
    IIC_Send_Byte(WriteAddr%256); //发送低地址
    IIC_Wait_Ack();
    
    IIC_Send_Byte(DataToWrite); //发送字节
    IIC_Wait_Ack();
    
    IIC_Stop();//产生一个停止条件
    AT24CXX_DELAY(10000);   //延迟10ms
}

/*******************************************************************
**  在 AT24CXX 里面的指定地址开始写入长度为 Len 的数据
**  该函数用于写入 16bit 或者 32bit 的数据.
**  WriteAddr :开始写入的地址
**  DataToWrite:数据数组首地址
**  Len :要写入数据的长度 2,4
********************************************************************/
void AT24CXX_WriteLenByte(unsigned short int WriteAddr,unsigned int DataToWrite,unsigned char Len)
{
    unsigned char t;
    for(t=0; t<Len; t++)
    {
        AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);    //低位写入低地址
    }
}

/*******************************************************************
**  在 AT24CXX 里面的指定地址开始读出长度为 Len 的数据
**  该函数用于读出 16bit 或者 32bit 的数据.
**  ReadAddr :开始读出的地址
**  返回值 :数据
**  Len :要读出数据的长度 2,4
********************************************************************/
unsigned int AT24CXX_ReadLenByte(unsigned short int ReadAddr, unsigned char Len)
{
    unsigned char t;
    unsigned int temp=0;
    for(t=0;t<Len;t++)
    {
        temp<<=8;
        temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1);    //读出时先读高位
    }
    return temp;
}

/*******************************************************************
**  检查 AT24CXX 是否正常
**  这里用了 24XX 的最后一个地址(255)来存储标志字.
**  如果用其他 24C 系列,这个地址要修改
**  返回 1:检测失败
**  返回 0:检测成功
********************************************************************/
unsigned char AT24CXX_Check(void)
{
    unsigned char temp;
    temp=AT24CXX_ReadOneByte(255);//避免每次开机都写 AT24CXX
    if(temp==0X0d)  return 0;
    else//排除第一次初始化的情况
    {
        AT24CXX_WriteOneByte(255,0X0d);
        temp=AT24CXX_ReadOneByte(255);
        if(temp==0X0d)  return 0;
    }
    return 1;
}

/*******************************************************************
**  在 AT24CXX 里面的指定地址开始读出指定个数的数据
**  ReadAddr :开始读出的地址 对 24c02 为 0~255
**  pBuffer :数据数组首地址
**  NumToRead:要读出数据的个数
**  
********************************************************************/
void AT24CXX_Read(unsigned short int ReadAddr,unsigned char *pBuffer,unsigned short int NumToRead)
{
    while(NumToRead)
    {
        *pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);
        NumToRead--;
    }
}

/*******************************************************************
**  在 AT24CXX 里面的指定地址开始写入指定个数的数据
**  WriteAddr :开始写入的地址 对 24c02 为 0~255
**  pBuffer :数据数组首地址
**  NumToWrite:要写入数据的个数
**  
********************************************************************/
void AT24CXX_Write(unsigned short int WriteAddr,unsigned char *pBuffer,unsigned short int NumToWrite)
{
    while(NumToWrite--)
    {
        AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}




