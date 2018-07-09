
#ifndef SPI_H
#define SPI_H

	#include "stm32f10x.h"
  #include "stm32f10x_spi.h"
		
	#define SPI1_FLASH_CS_LOW()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)
	#define SPI1_FLASH_CS_HIGH()  GPIO_SetBits(GPIOA, GPIO_Pin_4)
	
	void SPI1_Config(void);
	void SPI1_GPIO_Config(void);
	
	void SPI_Config(void);
	unsigned char SPI_Flash_SendByte(unsigned char byte);
	unsigned char SPI_Flash_ReadByte(unsigned char byte);
#endif
