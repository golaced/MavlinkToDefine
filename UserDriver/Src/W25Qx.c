
#include "W25Qx.h"

/******************************************
* 读取SPI_FLASH的状态寄存器
* BIT7  6   5   4   3   2   1   0
* SPR   RV  TB BP2 BP1 BP0 WEL BUSY
* SPR: 默认为 0,状态寄存器保护位，配合WP使用
//TB,BP2,BP1,BP0: FLASH写保护设置
//WEL: 写使能锁定
//BUSY: 忙标记位(1,忙;0,空闲)
//默认: 0x00
********************************************/
unsigned char SPI_W25Qx_ReadSR(void)
{ 
	unsigned char byte=0;
	W25Qx_FLASH_CS_LOW();	//片选                          
	SPI_Flash_SendByte(W25X_CMD_ReadStatusReg1); 	//读寄存器 1 指令   
	byte=SPI_Flash_ReadByte(W25X_DUMMY_BYTE);  		//读取数据   
	W25Qx_FLASH_CS_HIGH();  
	return byte;  
}

//*************************************************************************
//写SPI_FLASH状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写
void SST25_Page_Program_SR(unsigned char sr)
{  
	SPI_W25Qx_Write_Enable();  //写使能
	                       
	W25Qx_FLASH_CS_LOW();	//片选
	SPI_Flash_SendByte( W25X_CMD_WriteStatusReg ); //发送写状态寄存器指令
	SPI_Flash_SendByte( sr );	//写入一个字节
	W25Qx_FLASH_CS_HIGH();	            
}  

/********************************************
*   W25QX Flash芯片读取设备ID
*
********************************************/
unsigned int SPI_ReadDeviceID(void)
{
	unsigned int hb, lb;
	
	W25Qx_FLASH_CS_LOW();	//片选
	
	SPI_Flash_SendByte(W25X_ManufactDeviveID);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	SPI_Flash_SendByte(0x00);
	
	hb = SPI_Flash_ReadByte(W25X_DUMMY_BYTE);
	lb = SPI_Flash_ReadByte(W25X_DUMMY_BYTE);
	
	W25Qx_FLASH_CS_HIGH();		
	
	return ( hb<<8 | lb );	
}

/********************************************
*   W25QX Flash芯片写使能
*
********************************************/
void SPI_W25Qx_Write_Enable(void)
{
	W25Qx_FLASH_CS_LOW();	//片选
	
	SPI_Flash_SendByte(W25X_CMD_WriteEnable);
	
	W25Qx_FLASH_CS_HIGH();	
}

/********************************************
*   W25QX Flash芯片写失能
*
********************************************/
void SPI_W25Qx_Write_Disable(void)
{
	W25Qx_FLASH_CS_LOW();	//片选
	
	SPI_Flash_SendByte(W25X_CMD_WriteDisable);
	
	W25Qx_FLASH_CS_HIGH();	
}

/********************************************
*   W25QX 等待操作完成
*
********************************************/
void SPI_W25Qx_WaitForWriteEnd(void)
{
	unsigned int j;
	while(1)
	{	
		if( (SPI_W25Qx_ReadSR() & 0x01) == 0x00 )
		{
			return;
		}
		for( j=0; j<1000; ++j );
	}
}

/********************************************
*   W25QX 页写入操作 
*   输入地址无效返回0，成功返回1
********************************************/
unsigned char SPI_W25Qx_WritePage(unsigned char* pBuffer, unsigned int WriteAddr, unsigned int NumByteToWrite)
{
	if( (WriteAddr+NumByteToWrite)>W25X_SPI_SIZE || NumByteToWrite==0 )
	{
		return 0;		//地址无效则返回0 
	}
	/* Enable the write access to the FLASH */
	SPI_W25Qx_Write_Enable();

	/* Select the FLASH: Chip Select low */
	W25Qx_FLASH_CS_LOW();
	/* Send &quot;Write to Memory &quot; instruction */
	SPI_Flash_SendByte(W25X_CMD_PageProgram);
	/*Send WriteAddr high nibble address byte to write to */
	SPI_Flash_SendByte( (unsigned char)((WriteAddr&0xFF0000)>>16) ) ;
	/* Send WriteAddr medium nibble address byte to write to */
	SPI_Flash_SendByte( (unsigned char)((WriteAddr&0xFF00)>>8) );
	/* Send WriteAddr low nibble address byte to write to */
	SPI_Flash_SendByte( (unsigned char)(WriteAddr& 0xFF) );

	/* while there is data to be written on the FLASH */
	while (NumByteToWrite--)
	{
		/* Send the current byte */
		SPI_Flash_SendByte(*pBuffer);
		/* Point on the next byte to be written */
		pBuffer++;
	}

	/*Deselect the FLASH: Chip Select high */
	W25Qx_FLASH_CS_HIGH();

	/* Wait the end of Flash writing */
	SPI_W25Qx_WaitForWriteEnd();
	
	return 1;
}

/********************************************
*   W25QX 读出数据
*   输入地址无效返回0，成功返回1
********************************************/
unsigned char SPI_W25Qx_ReadBuffer(unsigned char* pBuffer, unsigned int ReadAddr, unsigned int NumByteToRead)
{
	if( (ReadAddr+NumByteToRead)>W25X_SPI_SIZE || NumByteToRead==0 )
	{
		return 0;		//地址无效则返回0 
	}
	/*Select the FLASH: Chip Select low */
	W25Qx_FLASH_CS_LOW();	 
	/*Send &quot;Read from Memory &quot; instruction */
	SPI_Flash_SendByte( W25X_CMD_FastRead );	 
	/*!&lt; Send ReadAddr high nibble address byte to read from */
	SPI_Flash_SendByte( (ReadAddr&0xFF0000)>>16 );
	/*!&lt; Send ReadAddr medium nibble address byte to read from */
	SPI_Flash_SendByte( (ReadAddr&0xFF00)>>8 );
	/*!&lt; Send ReadAddr low nibble address byte to read from */
	SPI_Flash_SendByte( ReadAddr&0xFF );
	 
	while (NumByteToRead--) /* while there is data to be read */
	{
		/*!&lt; Read a byte from the FLASH */
		*pBuffer = SPI_Flash_SendByte(W25X_DUMMY_BYTE);
		/*!&lt; Point to the next location where the byte read will be saved */
		pBuffer++;
	}
	 
	/*!&lt; Deselect the FLASH: Chip Select high */
	W25Qx_FLASH_CS_HIGH();
	return 0;
}

/********************************************
*    擦除整片 Flash 数据
*
********************************************/
void SPI_W25Qx_EraseChip(void)
{
	/* Send write enable instruction */
	SPI_W25Qx_Write_Enable();
	SPI_W25Qx_WaitForWriteEnd();
	/* Bulk Erase */
	/* Select the FLASH: Chip Select low */
	W25Qx_FLASH_CS_LOW();
	/* Send Bulk Erase instruction */
	SPI_Flash_SendByte(W25X_CMD_ChipErase);
	/* Deselect the FLASH: Chip Select high */
	W25Qx_FLASH_CS_HIGH();
	 
	/* Wait the end of Flash writing */
	SPI_W25Qx_WaitForWriteEnd();
}

/********************************************
*    擦除一个扇区的数据  
*    Sector_Dst_Addr： 0-511
********************************************/
void SPI_W25Qx_SectorErase(unsigned int Sector_Dst_Addr)
{
	unsigned int add = W25X_SPI_SECTORSIZE*Sector_Dst_Addr;
	/* Send write enable instruction */
	SPI_W25Qx_Write_Enable();
	SPI_W25Qx_WaitForWriteEnd();
	/* Bulk Erase */
	/* Select the FLASH: Chip Select low */
	W25Qx_FLASH_CS_LOW();
	/* Send Bulk Erase instruction */
	SPI_Flash_SendByte(W25X_CMD_SectorErase);
	/*!&lt; Send ReadAddr high nibble address byte to read from */
	SPI_Flash_SendByte( (add&0xFF0000)>>16 );
	/*!&lt; Send ReadAddr medium nibble address byte to read from */
	SPI_Flash_SendByte( (add&0xFF00)>>8 );
	/*!&lt; Send ReadAddr low nibble address byte to read from */
	SPI_Flash_SendByte( add&0xFF );
	/* Deselect the FLASH: Chip Select high */
	W25Qx_FLASH_CS_HIGH();
	 
	/* Wait the end of Flash writing */
	SPI_W25Qx_WaitForWriteEnd();
}
