#include "adc.h"

#define ADC1_DR_Address    ((u32)0x40012400+0x4c)
#define TEMP_SENSOR  ADC_Channel_16 //内部温度传感器AD采集通道
#define INNER_VOLTAGE	ADC_Channel_17			 //内部参考电压AD采集通道
#define CURRENT_COLLECTION	ADC_Channel_8  //对应IO口  PB8
#define AD_DMA_ENABLE DMA_Cmd(DMA1_Channel1, ENABLE);	
#define AD_DMA_DISABLE DMA_Cmd(DMA1_Channel1, DISABLE);	

#define INNER_VOL_REF	1.24  //内部参考电压值，通过该值和aDC采样值可以求得此时的VDDA值

#define ADC_SIZE 30
__IO uint16_t ADC_ConvertedValue[ADC_SIZE];  //采用DMA方式， 1-通道9-采集电流  2-通道16-采集内部温度  3-通道17-采集参考电压


/*
 * 函数名：ADC1_GPIO_Config
 * 描述  ：使能ADC1和DMA1的时钟，初始化PC.01
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void ADC1_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable DMA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOA, ENABLE);
	
	/* Configure PC.01  as analog input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);				// PC1,输入时不用设置速率
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 //PA5
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);				// PB1,输入时不用设置速率
}


/* 函数名：ADC1_Mode_Config
 * 描述  ：配置ADC1的工作模式为MDA模式
 * 输入  : 无
 * 输出  ：无
 * 调用  ：内部调用
 */
static void ADC1_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	/* DMA channel1 configuration */
		   
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;		        // 外设基地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue;	        // AD转换值所存放的内存基地址	（就是给个地址）
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                        // 外设作为数据传输的来源	
  DMA_InitStructure.DMA_BufferSize = ADC_SIZE;                                     // 定义指定DMA通道 DMA缓存的大小
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          // 外设地址寄存器不变
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			        // 内存地址寄存器不变
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 数据宽度为16位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;         // HalfWord
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;		                    //工作在循环模式下
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	                    //高优先级
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;		                        //没有设置为内存到内存的传输
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* Enable DMA channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);											//ENABLE她

  /* ADC1 configuration */
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                        //独立工作模式
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;			                    //多通道
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;			            //连续转换
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;       //由软件触发启动
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					//Right
  ADC_InitStructure.ADC_NbrOfChannel = 3;				                    //3个通道转换
  ADC_Init(ADC1, &ADC_InitStructure);
	
  	/*配置ADC时钟，为PCLK2的8分频，即9MHz*/
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 

  /* ADC1 regular channel16 configuration */ 
  //设置采样通道IN16, 设置采样时间
	ADC_RegularChannelConfig(ADC1, CURRENT_COLLECTION, 1, ADC_SampleTime_239Cycles5);	
	ADC_RegularChannelConfig(ADC1, TEMP_SENSOR, 2, ADC_SampleTime_239Cycles5);	
  ADC_RegularChannelConfig(ADC1, INNER_VOLTAGE, 3, ADC_SampleTime_239Cycles5);	

  //使能温度传感器和内部参考电压   
  ADC_TempSensorVrefintCmd(ENABLE);                                    

   /* Enable ADC1 DMA */	  
  ADC_DMACmd(ADC1, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);			                              
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));                         

  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);				                        
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));	  
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

/*
 * 函数名：ADC1_Init
 * 描述  ：无
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */
void ADC1_Init(void)
{
	ADC1_GPIO_Config();
	ADC1_Mode_Config();
}

/*
 * 函数名：getInnerTempValue()
 * 描述  ：无
 * 输入  ：采样得到的原始值
 * 输出  ：摄氏度
 * 调用  ：外部调用
 */
float getInnerTempValue(void)
{
	float c=0.0, v=0.0;
	unsigned char i = 0;
	unsigned int sum = 0;
	float max=0.0,min=0.0;	//原始值
	int imax=0,imin=0;			//扩大acc倍的取整数值
	int iadc = 0;
	int acc = 10;  //精度
	
	AD_DMA_DISABLE;
	
	imax = imin = (int)(ADC_ConvertedValue[1]*acc);
	
	for( ; i<ADC_SIZE/3; ++i)
	{
		iadc = (int)(ADC_ConvertedValue[1+i*3]*acc);
		imax = (int)(max*acc);
		imin = (int)(min*acc);
		if(iadc>imax)
		{
			max = ADC_ConvertedValue[1+i*3];
		}
		if(iadc<imin)
		{
			min = ADC_ConvertedValue[1+i*3];
		}
		sum += ADC_ConvertedValue[1+i*3];
	}
	AD_DMA_ENABLE;
	
	sum = sum-max-min;
	v = (((sum/((ADC_SIZE/3.0)-2.0)))/4095) * 3.3;
	c = (V25_TYPE-v)*1000/AVG_SLOPE;
	c += 25;
;	
	return c;
}

/*
 * 函数名：getInnerVolValue()
 * 描述  ：无
 * 输入  ：采样得到的原始值
 * 输出  ：内部电压
 * 调用  ：外部调用
 */
float getInnerVolValue(void)
{
	float v=0.0;
	unsigned char i = 0;
	unsigned int sum = 0;
	float max=0.0,min=0.0;	//原始值
	int imax=0,imin=0;			//扩大acc倍的取整数值
	int iadc = 0;
	int acc = 10;  //精度
	
	AD_DMA_DISABLE;
	
	imax = imin = (int)(ADC_ConvertedValue[2]*acc);
	
	for( ; i<ADC_SIZE/3; ++i)
	{
		iadc = (int)(ADC_ConvertedValue[2+i*3]*acc);
		imax = (int)(max*acc);
		imin = (int)(min*acc);
		if(iadc>imax)
		{
			max = ADC_ConvertedValue[2+i*3];
		}
		if(iadc<imin)
		{
			min = ADC_ConvertedValue[2+i*3];
		}
		sum += ADC_ConvertedValue[2+i*3];
	}
	AD_DMA_ENABLE;
	sum = sum-max-min;
	
	v = (INNER_VOL_REF*4095)/(sum/(ADC_SIZE/3.0-2));
	
	return v;
}

/*
 * 函数名：getCurrentValue()
 * 描述  ：无
 * 输入  ：采样得到的原始值
 * 输出  ：内部电压
 * 调用  ：外部调用
 */
float getCurrentValue(void)
{
	float v=0.0;
	unsigned char i = 0;
	unsigned int sum = 0;
	float max=0.0,min=0.0;	//原始值
	int imax=0,imin=0;			//扩大acc倍的取整数值
	int iadc = 0;
	int acc = 10;  //精度
	
	AD_DMA_DISABLE;
	max = min = ADC_ConvertedValue[0];
	imax = imin = (int)(ADC_ConvertedValue[0]*acc);
	
	for( ; i<ADC_SIZE/3; ++i)
	{
		iadc = (int)(ADC_ConvertedValue[0+i*3]*acc);
		imax = (int)(max*acc);
		imin = (int)(min*acc);
		if(iadc>imax)
		{
			max = ADC_ConvertedValue[0+i*3];
		}
		if(iadc<imin)
		{
			min = ADC_ConvertedValue[0+i*3];
		}
		sum += ADC_ConvertedValue[0+i*3];
	}
	AD_DMA_ENABLE;
	sum = sum-max-min;
	v = (((sum/((ADC_SIZE/3.0)-2.0)))/4095) * 3.3;
	
	return v;
}

/********************************************
*  浮点数转换成字符数组,自动去除字符串前面的0，不带四舍五入
*  参数： pc 字符数组指针地址
*					f  需要转换的浮点数
*					intn  整数位数
*         decn  小数位数
*******************************************/
void floatToString(char *pc, float f, unsigned char intn, unsigned char decn)
{
	char *pstr = 0;
	int num = 0;
	unsigned char i = 0;
	float dec = 0.0;
	unsigned char j = 0;
	
	if(0==pc || 0==intn)  return;
	
	pstr = pc;
	
	num = (int)f;
	if(num<0)   //负数
	{
		*pstr = '-';
		++pstr;
		num = -num;
	}
	num %= (unsigned int)pow(10, intn);
	
	for(i=intn; i>0; --i) //整数部分
	{
		*pstr = (num/(unsigned int)pow(10,i-1))+0x30;
		if(0x30==*pstr&&0==j)   //头部为0
		{
			continue;
		}
		j = 1;
		num = num%((unsigned int)pow(10, i-1));
		++pstr;
	}
	
	if(0==j)
	{
		*pstr = '0';
		++pstr;
	}
	
	if(0==decn)					//小数部分不保留，返回
	{
//		*pstr = '\r';
//		++pstr;
//		*pstr = '\n';
//		++pstr;
		*pstr = '\0';
		return;
	}
	
	*pstr = '.';	
	++pstr;
	
	dec = f-(int)f;       //取小数部分
	num = (int)(dec*pow(10, decn));
	if(num<0)
	{
		num = -num;
	}
	
	for(i=decn; i>0; --i)		//分离小数部分
	{
		*pstr = (num/pow(10,i-1))+0x30;
		num = num%((unsigned int)pow(10, i-1));
		++pstr;
	}	
//	*pstr = '\r';
//	++pstr;
//	*pstr = '\n';
//	++pstr;
	*pstr = '\0';
	
	
}
