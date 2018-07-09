
#ifndef USART_H
#define USART_H

	#include "stm32f10x.h"
	#include "FreeRTOS.h"
	#include "semphr.h"
	#include "queue.h"
	#include "LoopQueue.h"
	static u8 USART3_BuffReadEnable;
	
		/* 串口相关定义 */
	typedef struct _com_rx_struct
	{
		u8* buff;
		u8 count;
		u8 max_len;
	}com_rx_buff;
	extern com_rx_buff com1_rx_buff;
	extern com_rx_buff com3_rx_buff;

	void usart3_config(void);  //关于串口相关配置：IO口，串口参数
	void usart1_config(void);
	void usart2_config(void);
	void usartSendString(USART_TypeDef* const USARTx, const u8* pc);
	u8 getUsartBuffReadFlag(void); //获取处理标志
	void setUsartBuffReadFlag(u8 ui); //设置标志
	LoopQueue* getUsart3Queue(void);
	void usartSendChar(USART_TypeDef* const USARTx, unsigned char c);
	LoopQueue* getUsart1Queue(void);
	void usart2Config(void);
	unsigned char* getSendStatus(unsigned char index);
  void setSendStatus(unsigned char index, unsigned char flag);
  void comStartSendBuff(unsigned char chan, unsigned char* src, unsigned int count);
	
	void setUsartRxCount(unsigned char chan, unsigned int count);
	unsigned int getUsartRxCount(unsigned char chan, unsigned char clean);

	unsigned int* getSendDataCount(unsigned char serial_dma);
	void setSendDataCount(unsigned char serial_dma,unsigned int count);
	void serialSendData(unsigned char serial_dma, unsigned int len);
	void setSendEnableStatus(unsigned char serial_dma, unsigned char enbale);
	unsigned char* getSendEnableStatus(unsigned char serial_dma);
	void insertDataToBuff(unsigned char serial_dma, const char* buff, unsigned int len);
	
	void byteToHEX(unsigned char c, unsigned char* pc);
	void copyToBuff(char* dest, const char* src, unsigned int len);

	
#endif
