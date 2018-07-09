
#ifndef TFT_DRIVER_H
#define TFT_DRIVER_H

	#include "stm32f10x.h"
	
	/* 写TFT寄存器地址 */
	#define LCD_WR_REG(add)    ((*(volatile u16 *)((uint32_t)0x60000000)) = ((u16)add))
	/* 给TFT寄存器写数据 */
	#define LCD_WR_DATA(val)       ((*(volatile u16 *) ((uint32_t)0x60020000)) = ((u16)(val)))
	
	/*块写入指令， 行，列指令*/
	#define XPOSITION   0x2A
	#define YPOSITION   0x2B	
	
	/* 写入显示数据 */
	#define DATAREG 	0X2C
		
	void TFT_Delay(unsigned int val);
	void TFT_FSMC_Init(void);
	void TFT_IO_Init(void);
	void TFT_Init(void);
	void TFT_DrawPoint(int x, int y, int color);
	unsigned short int  TFT_ReadPoint(int x, int y);
#endif
