#ifndef __ILI9341_H
#define __ILI9341_H

#include "stm32f10x.h"
#include "ili9341.h"
#include "font.h"

/* Ð´TFT¼Ä´æÆ÷µØÖ· */
#define LCD_WR_REG(add)    ((*(volatile u16 *)((uint32_t)0x60000000)) = ((u16)add))
/* ¸øTFT¼Ä´æÆ÷Ð´Êý¾Ý */
#define LCD_WR_DATA(val)       ((*(volatile u16 *) ((uint32_t)0x60020000)) = ((u16)(val)))

void LCD_XYRAM(uint16_t stx,uint16_t sty,uint16_t endx,uint16_t endy);
void TFT_GPIO_Init(void);
void FSMC_Init(void);
void ILI9341_Iint(void);
void LCD_Clear(uint16_t color);
void diplayChar(uint16_t color);


#endif




