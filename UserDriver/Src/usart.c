/**************************************************************
*		硬件串口配置
*		串口1： 9600, 8, N, 1
*		串口2： NONE
*	  串口3： 9600，8，N, 1  有缓存区，支持队列操作
**************************************************************/

#include "usart.h"
#include "task.h"

/* 外部变量声明 */
extern xQueueHandle com1_tx_queue_handle;
extern xQueueHandle com1_rx_queue_handle;
extern xQueueHandle com3_rx_queue_handle;
extern xQueueHandle com3_tx_queue_handle;
extern xQueueHandle com2_tx_queue_handle;
extern xQueueHandle com2_rx_queue_handle;

/* 串口配置中断、DMA等宏定义 */
#define USART1_SEND_DMA_ENABLE 	0
#define USART1_REC_DMA_ENABLE 	0
#define USART2_SEND_DMA_ENABLE 	0
#define USART2_REC_DMA_ENABLE   0
#define USART3_SEND_DMA_ENABLE 	0
#define USART3_REC_DMA_ENABLE		0

#if USART1_SEND_DMA_ENABLE
	#define USART1_TX_BUFF_LEN 256
	u8 usart1_tx_dma_buff[USART1_TX_BUFF_LEN]; 
#endif

#if USART2_SEND_DMA_ENABLE
	#define USART2_TX_BUFF_LEN 256
	u8 usart2_tx_dma_buff[USART1_TX_BUFF_LEN]; 
#endif

#if USART3_SEND_DMA_ENABLE
	#define USART3_TX_BUFF_LEN 256
	u8 usart3_tx_dma_buff[USART3_TX_BUFF_LEN]; 
#endif



/*************************************************************
* 串口1配置:
* 				时钟使能、引脚配置、中断优先级配置
**************************************************************/
void usart1_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef nvicStruct;
	
	/* 时钟初始化 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
	
	/* IO引脚初始化 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 初始化串口参数 */
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	
	USART_Init(USART1, &USART_InitStructure);
	
	/* 串口1中断 */
	nvicStruct.NVIC_IRQChannel = USART1_IRQn;
	nvicStruct.NVIC_IRQChannelPreemptionPriority = 9;
	nvicStruct.NVIC_IRQChannelSubPriority = 0;
	nvicStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStruct); 
	
	/* 串口1接收DMA通道  通道DMA1-5 */
	#if USART1_REC_DMA_ENABLE
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    
		DMA_DeInit(DMA1_Channel5);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART1->DR);	 				//外设寄存器地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)usart1_rx_dma_buff; 				//DMA内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;        							//外设作为数据的来源
		DMA_InitStructure.DMA_BufferSize = USART1_RX_BUFF_LEN;			  					//缓冲区大小
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设寄存器地址不增加
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			  					//内存地址自增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据宽度为8位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//内存数据宽度为8位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	   											//工作在循环缓冲模式下
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;										//通道5位中等优先级
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;														//通道5不设置内存到内存的传输
		DMA_Init(DMA1_Channel5, &DMA_InitStructure);
		/* DMA1-5 中断 */
		nvicStruct.NVIC_IRQChannel = DMA1_Channel5_IRQn;	
		nvicStruct.NVIC_IRQChannelPreemptionPriority = 0;
		nvicStruct.NVIC_IRQChannelSubPriority = 0;
		nvicStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicStruct); 
		
		DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);		//开启DMA1传输完成中断
		USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);		//使能串口USART1的DMA
		DMA_Cmd(DMA1_Channel5,DISABLE); 
	#endif
	
		/***  串口1发送DMA通道  通道DMA1-4  ***/
	#if USART1_SEND_DMA_ENABLE==1																							//串口2使用DMA发送配置
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    
		DMA_DeInit(DMA1_Channel4);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART1->DR);					//外设寄存器地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)usart1_tx_dma_buff; 				//DMA内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;        							//外设作为数据的来源
		DMA_InitStructure.DMA_BufferSize = USART1_TX_BUFF_LEN;			  					//缓冲区大小
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设寄存器地址不增加
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			  					//内存地址自增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据宽度为8位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//内存数据宽度为8位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	   												//工作在一般模式下
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;									//通道4中等优先级
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;														//通道4不设置内存到内存的传输
		DMA_Init(DMA1_Channel4, &DMA_InitStructure);			
		/*中断*/
		nvicStruct.NVIC_IRQChannel = DMA1_Channel4_IRQn;	
		nvicStruct.NVIC_IRQChannelPreemptionPriority = 0;
		nvicStruct.NVIC_IRQChannelSubPriority = 1;
		nvicStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicStruct); 
		
		DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);		//开启DMA1传输完成中断
		USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
		DMA_Cmd(DMA1_Channel4,DISABLE); 
	#endif	
	
	#if USART1_REC_DMA_ENABLE!=1
		USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	#endif
	USART_Cmd(USART1, DISABLE);
	
}

/*---------------------------------------------------------------------
 * USART2 配置,115200 8-N-1	   
 */
void usart2_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef nvicStruct;
	
	/* 时钟初始化 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	/* IO引脚初始化 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* 串口参数初始化 */  
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;		
	USART_Init(USART2, &USART_InitStructure);
	
	/* 串口2中断 */
	nvicStruct.NVIC_IRQChannel = USART2_IRQn;
	nvicStruct.NVIC_IRQChannelPreemptionPriority = 8;
	nvicStruct.NVIC_IRQChannelSubPriority = 0;
	nvicStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStruct); 
	
	/* 串口2接收DMA通道  通道DMA1-6 */
	#if USART2_REC_DMA_ENABLE
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    
		DMA_DeInit(DMA1_Channel6);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART1->DR);	 				//外设寄存器地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)usart2_rx_dma_buff; 				//DMA内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;        							//外设作为数据的来源
		DMA_InitStructure.DMA_BufferSize = USART2_RX_BUFF_LEN;			  					//缓冲区大小
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设寄存器地址不增加
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			  					//内存地址自增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据宽度为8位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//内存数据宽度为8位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	   											//工作在循环缓冲模式下
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;										//通道5位中等优先级
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;														//通道5不设置内存到内存的传输
		DMA_Init(DMA1_Channel6, &DMA_InitStructure);
		/* DMA1-5 中断 */
		nvicStruct.NVIC_IRQChannel = DMA1_Channel6_IRQn;	
		nvicStruct.NVIC_IRQChannelPreemptionPriority = 0;
		nvicStruct.NVIC_IRQChannelSubPriority = 1;
		nvicStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicStruct); 
		
		DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);		//开启DMA1传输完成中断
		USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);		//使能串口USART2的DMA
		DMA_Cmd(DMA1_Channel6,DISABLE); 
	#endif
	
		/***  串口1发送DMA通道  通道DMA1-4  ***/
	#if USART2_SEND_DMA_ENABLE==1																							//串口2使用DMA发送配置
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    
		DMA_DeInit(DMA1_Channel7);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART2->DR);					//外设寄存器地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)usart2_tx_dma_buff; 				//DMA内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;        							//外设作为数据的来源
		DMA_InitStructure.DMA_BufferSize = USART2_TX_BUFF_LEN;			  					//缓冲区大小
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设寄存器地址不增加
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			  					//内存地址自增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据宽度为8位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//内存数据宽度为8位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	   												//工作在一般模式下
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;									//通道4中等优先级
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;														//通道4不设置内存到内存的传输
		DMA_Init(DMA1_Channel7, &DMA_InitStructure);			
		/*中断*/
		nvicStruct.NVIC_IRQChannel = DMA1_Channel7_IRQn;	
		nvicStruct.NVIC_IRQChannelPreemptionPriority = 1;
		nvicStruct.NVIC_IRQChannelSubPriority = 1;
		nvicStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicStruct); 
		
		DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);		//开启DMA1传输完成中断
		USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
		DMA_Cmd(DMA1_Channel7,DISABLE); 
	#endif	
	
	#if USART2_REC_DMA_ENABLE!=1
		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	#endif
	USART_Cmd(USART2, DISABLE);	
	   
}

/*************************************************************
* 串口3配置:
* 				时钟使能、引脚配置、中断优先级配置
**************************************************************/
void usart3_config(void)
{
	USART_InitTypeDef usartStruct;
	GPIO_InitTypeDef GPIOInit;
  NVIC_InitTypeDef nvicStruct;
	DMA_InitTypeDef DMA_InitStructure;
	
	/* 时钟初始化 */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART3, ENABLE );
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO , ENABLE );	
	
	/* 引脚IO配置 */
	GPIOInit.GPIO_Pin = GPIO_Pin_11;						
	GPIOInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 	
	GPIO_Init(GPIOB, &GPIOInit);
	
	GPIOInit.GPIO_Pin = GPIO_Pin_10; 					
	GPIOInit.GPIO_Mode = GPIO_Mode_AF_PP; 				
	GPIOInit.GPIO_Speed = GPIO_Speed_50MHz;				
	GPIO_Init(GPIOB, &GPIOInit);
			
	/*** 初始化串口参数	***/
	usartStruct.USART_BaudRate = 115200;    								
	usartStruct.USART_WordLength = USART_WordLength_8b;    	
	usartStruct.USART_StopBits = USART_StopBits_1;					
	usartStruct.USART_Parity = USART_Parity_No;							
	usartStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   
	usartStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;						  		
	USART_Init(USART3, &usartStruct);	
		
	/* 串口3中断 */
	nvicStruct.NVIC_IRQChannel = USART3_IRQn;
	nvicStruct.NVIC_IRQChannelPreemptionPriority = 7;
	nvicStruct.NVIC_IRQChannelSubPriority = 0;
	nvicStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStruct); 
	
		/* 串口3接收DMA通道  通道DMA1-3 */
	#if USART3_REC_DMA_ENABLE
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    
		DMA_DeInit(DMA1_Channel3);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART1->DR);	 				//外设寄存器地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)usart3_rx_dma_buff; 				//DMA内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;        							//外设作为数据的来源
		DMA_InitStructure.DMA_BufferSize = USART3_RX_BUFF_LEN;			  					//缓冲区大小
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设寄存器地址不增加
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			  					//内存地址自增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据宽度为8位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//内存数据宽度为8位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	   											//工作在循环缓冲模式下
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;										//通道3位中等优先级
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;														//通道3不设置内存到内存的传输
		DMA_Init(DMA1_Channel3, &DMA_InitStructure);
		/* DMA1-3 中断 */
		nvicStruct.NVIC_IRQChannel = DMA1_Channel3_IRQn;	
		nvicStruct.NVIC_IRQChannelPreemptionPriority = 1;
		nvicStruct.NVIC_IRQChannelSubPriority = 1;
		nvicStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicStruct); 
		
		DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);		//开启DMA1传输完成中断
		USART_DMACmd(USART3, USART_DMAReq_Rx, ENABLE);		//使能串口USART1的DMA
		DMA_Cmd(DMA1_Channel3,DISABLE); 
	#endif
	
		/***  串口3发送DMA通道  通道DMA1-2  ***/
	#if USART3_SEND_DMA_ENABLE==1																							//串口2使用DMA发送配置
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);    
		DMA_DeInit(DMA1_Channel2);
		DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&(USART3->DR);					//外设寄存器地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)usart3_tx_dma_buff; 				//DMA内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;        							//外设作为数据的来源
		DMA_InitStructure.DMA_BufferSize = USART3_TX_BUFF_LEN;			  					//缓冲区大小
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				//外设寄存器地址不增加
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			  					//内存地址自增
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//外设数据宽度为8位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//内存数据宽度为8位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;	   												//工作在一般模式下
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;									//通道2中等优先级
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;														//通道2不设置内存到内存的传输
		DMA_Init(DMA1_Channel2, &DMA_InitStructure);			
		/*中断*/
		nvicStruct.NVIC_IRQChannel = DMA1_Channel2_IRQn;	
		nvicStruct.NVIC_IRQChannelPreemptionPriority = 1;
		nvicStruct.NVIC_IRQChannelSubPriority = 1;
		nvicStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvicStruct); 
		
		DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);		//开启DMA1传输完成中断
		USART_DMACmd(USART3, USART_DMAReq_Tx, ENABLE);
		DMA_Cmd(DMA1_Channel2,DISABLE); 
	#endif
	
	#if USART3_REC_DMA_ENABLE!=1
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		USART_ITConfig(USART3, USART_IT_TC, DISABLE);
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	#endif
	USART_Cmd(USART3, DISABLE);

}
//////////////////////////////////////////////////////////////
//	窗口采用DMA发送相关函数
//               
/////////////////////////////////////////////////////////////

/*************************************************************
* 设置串口发送数据个数，DMA方式发送
* 参数： serial_dma为串口对应的通道  1、2、3分别对应串口1、2、3
* 			 count为数据个数
**************************************************************/
void insertDataToBuff(unsigned char serial_dma, const char* buff, unsigned int len)
{
	unsigned int* pi = 0;
	unsigned char* pc = 0;
	unsigned int i = 0;
  unsigned int j = 0;
	
	while(i<5000)
	{
		pc = getSendEnableStatus(serial_dma);   //判断是否允许发送
		if(*pc)  //允许发送数据，则向缓冲区中写入数据
		{
			if(1==serial_dma)
			{
				#if USART1_SEND_DMA_ENABLE==1		//串口1使用DMA发送配置
					copyToBuff(usart1_tx_dma_buff, buff, len);		//将数据复制到缓冲区	
				#endif
			}else if(2==serial_dma)
			{
				#if USART2_SEND_DMA_ENABLE==1
					copyToBuff(usart2_tx_dma_buff, buff, len);		//将数据复制到缓冲区	
				#endif
			}else if(3==serial_dma)
			{
				#if USART3_SEND_DMA_ENABLE==1
					copyToBuff(usart3_tx_dma_buff, buff, len);		//将数据复制到缓冲区	
				#endif
			}
			serialSendData(serial_dma,len);					//发送数据
			break;
		}
		++i;
	}
}

/*************************************************************
* 设置串口发送数据个数，DMA方式发送
* 参数： serial_dma为串口对应的通道  1、2、3分别对应串口1、2、3
* 			 count为数据个数
**************************************************************/
void setSendDataCount(unsigned char serial_dma, unsigned int count)
{
	unsigned int* pi = 0;
	if(serial_dma>=4) return;
	pi = getSendDataCount(serial_dma);
	if(0==pi) return;
	
	*pi = count;
}

/*************************************************************
* 获取需要串口发送数据的个数，DMA方式发送
* 参数： serial_dma为串口对应的通道  1、2、3分别对应串口1、2、3
**************************************************************/
unsigned int* getSendDataCount(unsigned char serial_dma)
{
	static unsigned int dma_count[3] = {0};   /*对应DMA通道将要发送的数据个数*/
	if(serial_dma>=4) return 0;
		
	return &dma_count[serial_dma-1];
}

/*************************************************************
* 设置串口 DMA通道使能标志
* 参数： serial_dma为串口对应的通道  1、2、3分别对应串口1、2、3
* 			 count为数据个数
**************************************************************/
void setSendEnableStatus(unsigned char serial_dma, unsigned char enable)
{
	unsigned char* pi = 0;
	if(serial_dma>=4) return;
	pi = getSendEnableStatus(serial_dma);
	if(0==pi) return;
	
	*pi = enable;
}

/*************************************************************
* 获取需要串口发送数据的个数，DMA方式发送
* 参数： serial_dma为串口对应的通道  1、2、3分别对应串口1、2、3
**************************************************************/
unsigned char* getSendEnableStatus(unsigned char serial_dma)
{
	static unsigned char serial_dma_enbale[3] = {1,1,1}; //串口允许DMA发送标志，默认情况下使能; 
	
	if(serial_dma>=4) return 0;
		
	return &serial_dma_enbale[serial_dma-1];
}

/*************************************************************
* 设置串口发送数据个数，DMA方式发送
* 参数： serial_dma为串口对应的通道  1、2、3分别对应串口1、2、3
**************************************************************/
void serialSendData(unsigned char serial_dma,unsigned int len)
{	
	if(1==serial_dma)
	{
		#if USART1_SEND_DMA_ENABLE==1
			DMA_SetCurrDataCounter(DMA1_Channel4,len); 
			DMA_Cmd(DMA1_Channel4,ENABLE); 						
		#endif
	}else if(2==serial_dma)
	{
		#if USART2_SEND_DMA_ENABLE==1
			DMA_SetCurrDataCounter(DMA1_Channel7,len); 
			DMA_Cmd(DMA1_Channel7,ENABLE); 		
		#endif
	}else if(3==serial_dma)
	{
		#if USART3_SEND_DMA_ENABLE==1
			DMA_SetCurrDataCounter(DMA1_Channel2,len); 
			DMA_Cmd(DMA1_Channel2,ENABLE); 		
		#endif
	}	
	setSendEnableStatus(serial_dma,0);
}

/*************************************************************
* DMA1中断，串口1发送数据  DMA 2通道中断
* 
**************************************************************/
void DMA1_Channel2_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC2)==SET)
	{
		taskENTER_CRITICAL();
		setSendEnableStatus(3,1);   			//串口1发送数据完成
		setSendDataCount(3,0);						//重置数据有效个数
		taskEXIT_CRITICAL();
		DMA_Cmd(DMA1_Channel2,DISABLE);		//关闭DMA传输
		DMA_ClearFlag(DMA1_FLAG_TC2);			//清除中断标志	
	}		
}

/*************************************************************
* DMA1中断，串口1发送数据  DMA4通道中断
* 
**************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetFlagStatus(DMA1_FLAG_TC4)==SET)
	{
		taskENTER_CRITICAL();
		setSendEnableStatus(1,1);   			//串口1发送数据完成
		setSendDataCount(1,0);						//重置数据有效个数
		taskEXIT_CRITICAL();
		DMA_Cmd(DMA1_Channel4,DISABLE);		//关闭DMA传输
		DMA_ClearFlag(DMA1_FLAG_TC4);			//清除中断标志	
	}		
}

/*************************************************************
* DMA1中断，串口2发送数据  DMA7通道中断
* 
**************************************************************/
void DMA1_Channel7_IRQHandler(void)
{
//	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	if(DMA_GetFlagStatus(DMA1_FLAG_TC7)==SET)
	{
		taskENTER_CRITICAL();
		setSendEnableStatus(2,1);   			//串口2发送数据完成
		setSendDataCount(2,0);						//重置数据有效个数
		taskEXIT_CRITICAL();
		DMA_Cmd(DMA1_Channel7,DISABLE);		//关闭DMA传输
		DMA_ClearFlag(DMA1_FLAG_TC7);			//清除中断标志	
	}		
}

///////////////////////////////////////////////////////
//   结束DMA相关函数
///////////////////////////////////////////////////////

/*************************************************************
* 软件查询方式发送字符
* 输入： 串口寄存器结构体， 字符串首地址
**************************************************************/
void usartSendChar(USART_TypeDef* const USARTx, unsigned char c)
{	

	USART_SendData(USARTx, c); //发送一个字符
	while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE)==RESET);  //检测发送寄存器为空
	USART_ClearFlag(USARTx,USART_FLAG_TXE);		//清空该标志位置		
		
}

/*************************************************************
* 软件查询方式发送字符串
* 输入： 串口寄存器结构体， 字符串首地址
**************************************************************/
void usartSendString(USART_TypeDef* const USARTx, const u8* pc)
{
	if(pc==0) 
	{
		return;
	}
	
	while(*pc!='\0')
	{
		USART_SendData(USARTx, *pc); //发送一个字符
		while(USART_GetFlagStatus(USARTx,USART_FLAG_TXE)==RESET);  //检测发送寄存器为空
		USART_ClearFlag(USARTx,USART_FLAG_TXE);		//清空该标志位置		
		
		++pc;
	}
}


/*************************************************************
* 串口3接收数据中断，每接收100个数据，则存入队列
* 队列收到数据，激活相关任务执行
**************************************************************/
void USART3_IRQHandler(void)
{
	u8 ch = 0;
	portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;  /* 往数据队列中写入数据，如果激活了更高优先级的任务，该值为pdTRUE */
	
	if(USART_GetFlagStatus(USART3, USART_FLAG_TC)==SET)
	{
		USART_ClearFlag(USART3, USART_FLAG_TC);
	}
	
	if(USART_GetFlagStatus(USART3, USART_FLAG_RXNE)==SET)
	{
		ch = USART3->DR;

		if(xQueueIsQueueFullFromISR(com3_rx_queue_handle)==pdFALSE) /* 判断队列是否还有空间 */
		{
			xQueueSendToBackFromISR(com3_rx_queue_handle, &ch, &pxHigherPriorityTaskWoken);	
		}

		USART_ClearFlag(USART3, USART_FLAG_RXNE);
		
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);		
		
	}
	
	if(USART_GetFlagStatus(USART3, USART_FLAG_ORE)==SET)
	{
		USART_ClearFlag(USART3, USART_FLAG_ORE);
	}
	
//	if(USART_GetITStatus(USART3,USART_IT_ORE) != RESET) 
//	{
//		unsigned char del = 0;
//		del = USART_ReceiveData(USART3); 
//		USART_ClearITPendingBit(USART3, USART_IT_ORE);    
//	}
}

/*************************************************************
* 串口2接收数据中断，每接收100个数据，则存入队列
* 队列收到数据，激活相关任务执行
**************************************************************/
void USART2_IRQHandler(void)
{
	u8 ch = 0;  
	portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;  /* 往数据队列中写入数据，如果激活了更高优先级的任务，该值为pdTRUE */
	
	if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE)==SET)
	{
		ch = USART2->DR;
		if(xQueueIsQueueFullFromISR(com2_rx_queue_handle)==pdFALSE)
		{
			xQueueSendToBackFromISR(com2_rx_queue_handle, &ch, &pxHigherPriorityTaskWoken);
		}			
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
		
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);		
	}
	
	if(USART_GetFlagStatus(USART2, USART_FLAG_TC)==SET)
	{
		USART_ClearFlag(USART2, USART_FLAG_TC);
	}
	
	if(USART_GetITStatus(USART2,USART_IT_ORE) != RESET) 
	{
		unsigned char del = 0;
		del = USART_ReceiveData(USART2); 
		USART_ClearITPendingBit(USART2, USART_IT_ORE);    
	}
}

/*************************************************************
* 串口1接收数据中断，每接收100个数据，则存入队列
* 队列收到数据，激活相关任务执行
**************************************************************/
void USART1_IRQHandler(void)
{
	u8 ch = 0;  
	portBASE_TYPE pxHigherPriorityTaskWoken = pdFALSE;  /* 往数据队列中写入数据，如果激活了更高优先级的任务，该值为pdTRUE */
	
	if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE)==SET)
	{
		ch = USART1->DR;
		xQueueSendToBackFromISR(com1_rx_queue_handle, &ch, &pxHigherPriorityTaskWoken);	
		
		USART_ClearFlag(USART1, USART_FLAG_RXNE);
		IWDG_Feed();
		
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);		
	}
	
	if(USART_GetFlagStatus(USART1, USART_FLAG_TC)==SET)
	{
		USART_ClearFlag(USART1, USART_FLAG_TC);
	}
	
	if(USART_GetITStatus(USART1,USART_IT_ORE) != RESET) 
	{
		unsigned char del = 0;
		del = USART_ReceiveData(USART1); 
		USART_ClearITPendingBit(USART1, USART_IT_ORE);    
	}
}


/*************************************************************
*	usart2-tx  dma send end status  
* return: 0 - busy, 1 - idle
*************************************************************/
unsigned char* getSendStatus(unsigned char index)
{
	static unsigned char status[3] = {1};   //默认为空闲状态 
	return &status[index];
}
void setSendStatus(unsigned char index, unsigned char flag)
{
	unsigned char *status;
	status = getSendStatus(index);
	*status = flag;
}
void comStartSendBuff(unsigned char chan, unsigned char* src, unsigned int count)
{
	unsigned int num;
	unsigned char *pc;
	unsigned char *psrc;
	
	if(0==src || 0==count)  return;
	switch(chan)
	{
		case 1:
		{
			break;
		}
		case 2:
		{
			#if USART2_SEND_DMA_ENABLE==1
				pc = usart2_tx_dma_buff;
				for(num=count; num>0; --num)
				{
					*pc = *psrc;
					++pc;
					++psrc;
				}	
	
				DMA_SetCurrDataCounter(DMA1_Channel7,count);  
				DMA_Cmd(DMA1_Channel7,ENABLE);
				setSendStatus(2,0);   //开启发送，忙状态
			#endif
			break;
		}
		case 3:
		{
			
			break;
		}
		default: break;
	}	

}


/*************************************************************
* 获取相关标志位
* 
**************************************************************/
u8 getUsartBuffReadFlag(void)
{
	return USART3_BuffReadEnable;
}

/*************************************************************
* 设置相关标志位
* 
**************************************************************/
void setUsartBuffReadFlag(u8 ui)
{
	USART3_BuffReadEnable = ui;
}

/*********************************************
*		数字转换成 HEX 
*
**********************************************/
void byteToHEX(unsigned char c, unsigned char* pc)
{
	unsigned char hb, lb;
	hb = lb = 0;
	
	hb = c / 16;
	lb = c % 16;
	
	if( hb<10 )
	{
		*pc = hb + 48;    //??????ASCII?
	} 
	else
	{
		*pc = hb - 10 + 65;	 //???????10???
	}
	
	++pc; 
	
	if( lb<10 )
	{
		*pc = lb + 48;    //??????ASCII?
	} 
	else
	{
		*pc = lb - 10 + 65;	 //???????10???
	}
	
	++pc;
	*pc = '\0';	
}

/*********************************************
*		复制数据
*
**********************************************/
void copyToBuff(char* dest, const char* src, unsigned int len)
{
	unsigned int i = 0;
	
	if(0==dest && 0==src && 0==len)  return;
	
	for(i=0; i<len&&'\0'!=*src; i++)
	{
		*dest = *src;
		++dest;
		++src;
	}
}
