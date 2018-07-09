
#ifndef W25QX_H
#define W25QX_H
	
	#include "SPI.h"
	
		#define W25Q64_DeviceID 0xEF16
 
	#define W25X_CMD_WriteEnable 0x06 /*Write enable instruction */
	#define W25X_CMD_WriteDisable 0x04 /*! Write to Memory Disable */
	#define W25X_CMD_WriteStatusReg 0x01 /* Write Status Register instruction */
	 
	#define W25X_CMD_PageProgram 0x02 /* Write enable instruction */
	#define W25X_CMD_QuadPageProgram 0x32 /* Write enable instruction */
	 
	#define W25X_CMD_BlockErase64 0xD8 /* Block 64k Erase instruction */
	#define W25X_CMD_BlockErase32 0x52 /* Block 32k Erase instruction */
	#define W25X_CMD_ChipErase 0xC7 /* Bulk Erase instruction */
	#define W25X_CMD_SectorErase 0x20 /* Sector 4k Erase instruction */
	#define W25X_CMD_EraseSuspend 0x75 /* Sector 4k Erase instruction */
	#define W25X_CMD_EraseResume 0x7a /* Sector 4k Erase instruction */
	 
	#define W25X_CMD_ReadStatusReg1 0x05 /* Read Status Register instruction */
	#define W25X_CMD_ReadStatusReg2 0x35 /* Read Status Register instruction */
	 
	#define W25X_CMD_High_Perform_Mode 0xa3
	#define W25X_CMD_Conti_Read_Mode_Ret 0xff
	 
	#define W25X_WakeUp 0xAB
	#define W25X_JedecDeviveID 0x9F /*Read identification */
	#define W25X_ManufactDeviveID 0x90 /* Read identification */
	#define W25X_ReadUniqueID 0x4B
	 
	#define W25X_Power_Down 0xB9 /*Sector 4k Erase instruction */
	 
	#define W25X_CMD_ReadData 0x03 /* Read from Memory instruction */
	#define W25X_CMD_FastRead 0x0b /* Read from Memory instruction */
	#define W25X_CMD_FastReadDualOut 0x3b /*Read from Memory instruction */
	#define W25X_CMD_FastReadDualIO 0xBB /* Read from Memory instruction */
	#define W25X_CMD_FastReadQuadOut 0x6b /* Read from Memory instruction */
	#define W25X_CMD_FastReadQuadIO 0xeb /* Read from Memory instruction */
	#define W25X_CMD_OctalWordRead 0xe3 /* Read from Memory instruction */
	 
	#define W25X_DUMMY_BYTE 0xff //0xA5
	
	/*	W25Q16 */
	#define W25X_SPI_PAGESIZE 256	//256BYTE
	#define W25X_SPI_SECTORSIZE 16*256		//4KB
	#define W25X_SPI_SECTORNUM	(W25X_SPI_SIZE/W25X_SPI_SECTORSIZE)  //512扇区
	#define W25X_SPI_SIZE 2*1024*1024	//2MB
	
	#define W25Qx_FLASH_CS_LOW()  SPI1_FLASH_CS_LOW()
	#define W25Qx_FLASH_CS_HIGH() SPI1_FLASH_CS_HIGH()
	
	unsigned int SPI_ReadDeviceID(void);	//读取设备ID
	void SPI_W25Qx_Write_Enable(void);		//flash写使能
	void SPI_W25Qx_Write_Disable(void);		//flash写失能	
	unsigned char SPI_W25Qx_ReadSR(void);	//读取状态寄存器
	void SST25_Page_Program_SR(unsigned char sr); //写状态寄存器
	void SPI_W25Qx_WaitForWriteEnd(void); //等待flash不处于忙状态
	
	void SPI_W25Qx_EraseChip(void);	//擦除flash
	void SPI_W25Qx_SectorErase(unsigned int ReadAddr); //指定地址擦除扇区
	
	unsigned char SPI_W25Qx_WritePage(unsigned char* pBuffer, unsigned int WriteAddr, unsigned int NumByteToWrite);	//将缓冲区的数据写入flash
	unsigned char SPI_W25Qx_ReadBuffer(unsigned char* pBuffer, unsigned int ReadAddr, unsigned int NumByteToRead);	//读取数据到缓冲区
	
#endif
