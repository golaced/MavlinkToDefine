
#include "spi.h"

void SPI_Config(void)
{
	SPI1_GPIO_Config();
	SPI1_Config();
}

/********************************************
*   SPI: 硬件引脚初始化
*
********************************************/
void SPI1_GPIO_Config(void)
{                 
	GPIO_InitTypeDef  GPIO_InitStructure;         
		
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE );			           

		/*	SPI1: CS推挽输出	PA4	*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;            
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;               
	GPIO_Init( GPIOA, &GPIO_InitStructure );  
	
	/*	SPI1: SCK, MOSI复用推挽输出	PA5  PA7	*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;        
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF_PP;                          
	GPIO_Init( GPIOA, &GPIO_InitStructure );  
	
	/*	SPI1: MISO浮动输入	PA6	*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;        
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_IN_FLOATING;                          
	GPIO_Init( GPIOA, &GPIO_InitStructure ); 
	                        
} 

/********************************************
*   SPI: 模式配置初始化
*
********************************************/
void SPI1_Config(void)
{
	SPI_InitTypeDef SPI_InitStruct;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1 , ENABLE );	
	
	SPI1_FLASH_CS_HIGH();
	
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //双线双向全双工
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;                      //主机模式
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;                  //8位数据帧模式
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;                        //时钟空闲时为高
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;                       //数据捕捉于第二个时钟沿
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;                          //软件控制片选信号
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;                 //数据传输从 MSB 位开始
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; //波特率分频值为2
	SPI_InitStruct.SPI_CRCPolynomial = 7;														//CRC 校验多项式为7位
	SPI_Init( SPI1, &SPI_InitStruct );
	
	SPI_Cmd( SPI1, ENABLE );
	SPI_Flash_SendByte(0xff);
	
}

/********************************************
*   SPI: SPI发送一个字节，并且读取一个数据
*
********************************************/
unsigned char SPI_Flash_SendByte(unsigned char byte)
{
/*! Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
 
/*!Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, byte);
 
/*! Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
 
/*! Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}
 
/********************************************
*   SPI: 读取一个数据
*
********************************************/
unsigned char SPI_Flash_ReadByte(unsigned char byte)
{
	return (SPI_Flash_SendByte(byte));
}
