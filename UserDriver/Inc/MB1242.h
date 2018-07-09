/* ============================================================================================
* Maxbotic MB1242 I2C library for ARM STM32F103xx Microcontrollers - Main header file 
* Has bit, byte and buffer I2C R/W functions
*  
*  Changelog:
           
** ========================================================================================= */

#ifndef _MB1242_H_
#define _MB1242_H_


#include "stm32f10x.h"
#include "stdint.h"
#include "usart.h"
#include "sim_i2c.h"


#define MB1242_I2C                 I2C1
#define MB1242_I2C_ADDRESS         0xE0  //8位地址，7位为0x70
#define MB1242_COMMAND_READ_RANGE  0x51


bool MB1242_StartRangeMeasure(void);
void MB1242_TakeRangeReading(u8* range);

#endif /* __MB1242_H */

/****END OF FILE****/
