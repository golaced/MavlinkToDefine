#ifndef _AT24CXX_H_
#define _AT24CXX_H_

#include "IIC.h"

#define EEPROM_TYPE     AT24C02 //实际中用到的EEPROM类型
/*  可以供选择的EEPROM类型 */
#define AT24C01     128
#define AT24C02     256
#define AT24C04     512
#define AT24C08     1024 
#define AT24C16     2048

/*  AT24CXX设备地址 */
#define AT24CXX_ADDRESS     0xA0 //与硬件的连接有关系
#define AT24CXX_READ        AT24CXX_ADDRESS|1    //读bit有效
#define AT24CXX_WRITE       AT24CXX_ADDRESS|0    //写bit有效
#define AT24CXX_DELAY(time)		delay(time)

/*	函数原型 */
void AT24CXX_Init(void);	
unsigned char AT24CXX_ReadOneByte(unsigned short int ReadAddr);
unsigned int AT24CXX_ReadLenByte(unsigned short int ReadAddr, unsigned char Len);

void AT24CXX_WriteOneByte(unsigned short int WriteAddr,unsigned char DataToWrite);
void AT24CXX_WriteLenByte(unsigned short int WriteAddr,unsigned int DataToWrite,unsigned char Len);

unsigned char AT24CXX_Check(void);

void AT24CXX_Read(unsigned short int ReadAddr,unsigned char *pBuffer,unsigned short int NumToRead);
void AT24CXX_Write(unsigned short int WriteAddr,unsigned char *pBuffer,unsigned short int NumToWrite);


#endif
