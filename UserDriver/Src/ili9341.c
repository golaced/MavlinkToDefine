#include "ili9341.h"

extern const unsigned char ascii8_16[93][16] ;

#define XPOSITION   0x2A
#define YPOSITION   0x2B
#define XNUM			320
#define YNUM			240	
#define DATAREG 	0X2C


const unsigned char gbk[24*3*12] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x0C,0x1F,0xFF,0x18,0x00,0x07,0x18,0x00,0x1C,
0xFF,0xC0,0x38,0x10,0xC0,0x70,0x30,0xC0,0x60,0x30,0xC0,0x60,0x21,0xBF,0xFF,0x61,0x80,0x60,0x73,0x80,0x60,0x1F,0x00,0x60,
0x07,0x00,0x60,0x0D,0xC0,0x60,0x18,0x60,0x60,0x70,0x00,0x60,0xC0,0x0F,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"好",0*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x0C,0x1F,0xFF,0x18,0x00,0x07,0x18,0x00,0x1C,
0xFF,0xC0,0x38,0x10,0xC0,0x70,0x30,0xC0,0x60,0x30,0xC0,0x60,0x21,0xBF,0xFF,0x61,0x80,0x60,0x73,0x80,0x60,0x1F,0x00,0x60,
0x07,0x00,0x60,0x0D,0xC0,0x60,0x18,0x60,0x60,0x70,0x00,0x60,0xC0,0x0F,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"好",1*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x30,0x08,0x0C,0x18,0x18,0x06,0x1C,0x30,0x02,0x08,0x40,
0x7F,0xFF,0xFF,0x60,0x00,0x03,0x60,0x00,0x03,0x07,0xFF,0xF0,0x00,0x00,0xE0,0x00,0x07,0x80,0x00,0x0C,0x00,0xFF,0xFF,0xFF,
0x00,0x0C,0x00,0x00,0x0C,0x00,0x00,0x0C,0x00,0x01,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"学",2*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFF,0xFE,0x00,0x00,0x0E,0x00,0x00,0x0E,
0x06,0x00,0x0E,0x01,0xC0,0x0E,0x00,0x70,0x0C,0x00,0x0C,0x0C,0x00,0x00,0x6C,0x00,0x07,0x8C,0x00,0x78,0x0C,0x0F,0x80,0x0C,
0x38,0x00,0x0C,0x00,0x00,0x0C,0x00,0x00,0x18,0x00,0x0F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"习",3*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"，",4*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFE,0x00,0x18,0x00,0x00,0x18,0x00,
0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0xFF,0xFF,0xFF,0x00,0x38,0x00,0x00,0x3C,0x00,0x00,0x64,0x00,0x00,0xC6,0x00,
0x01,0xC3,0x00,0x03,0x01,0x80,0x0E,0x00,0x60,0x38,0x00,0x38,0x60,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"天",5*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFE,0x00,0x18,0x00,0x00,0x18,0x00,
0x00,0x18,0x00,0x00,0x18,0x00,0x00,0x18,0x00,0xFF,0xFF,0xFF,0x00,0x38,0x00,0x00,0x3C,0x00,0x00,0x64,0x00,0x00,0xC6,0x00,
0x01,0xC3,0x00,0x03,0x01,0x80,0x0E,0x00,0x60,0x38,0x00,0x38,0x60,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"天",6*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x60,0x00,0x00,0x60,0x00,0x3F,0xFF,0xFE,
0x30,0x00,0x06,0x30,0x00,0x06,0x30,0x00,0x06,0x31,0xFF,0xC6,0x31,0x80,0xC6,0x31,0x80,0xC6,0x31,0x80,0xC6,0x31,0x80,0xC6,
0x31,0xFF,0xC6,0x31,0x80,0x06,0x30,0x00,0x06,0x30,0x00,0x06,0x30,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"向",7*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,
0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x3F,0xFC,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,
0x00,0x30,0x00,0x00,0x30,0x00,0x00,0x30,0x00,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"上",8*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,
0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x18,0x00,0x00,0x08,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"！",9*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,
0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x18,0x00,0x00,0x08,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"！",10*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,
0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x18,0x00,0x00,0x08,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"！",11*/
};

volatile void delay_tft(uint32_t val)
{
	val=val*5000;
	while(val--);
}

void FSMC_Init(void)
{
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p; 
    
    
    p.FSMC_AddressSetupTime = 0x02;	 //地址建立时间
    p.FSMC_AddressHoldTime = 0x00;	 //地址保持时间
    p.FSMC_DataSetupTime = 0x05;		 //数据建立时间
    p.FSMC_BusTurnAroundDuration = 0x00;
    p.FSMC_CLKDivision = 0x00;
    p.FSMC_DataLatency = 0x00;
    p.FSMC_AccessMode = FSMC_AccessMode_B;	 // 一般使用模式B来控制LCD
    
    FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
    //FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
		FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
    FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
    FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;  
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
    
    /* 使能 FSMC Bank1_SRAM Bank */
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

void TFT_GPIO_Init(void)
{
	/* 定义一个GPIO_IintTypeDef类型的变量 */
	GPIO_InitTypeDef GPIO_InitStructure;
	/* 使能FSMC时钟*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);
	/* 使能FSMC对应相应管脚时钟*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
	/* 配置FSMC引脚 */	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;// 设置为50M
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF_PP;// 设置为复用推挽输出模式
	/* 选择GPIOD口的引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | 
	                              GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_8 | 
	                              GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | 
	                              GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	/* 选择GPIOE口的引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | 
	                              GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | 
	                              GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
  /* 配置好FSMC功能 */	
	FSMC_Init();
}



void ILI9341_Iint(void)
{
	TFT_GPIO_Init();
	delay_tft(200);
	
	LCD_WR_REG(0xCF);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0xC1); 
	LCD_WR_DATA(0X30);
	
	LCD_WR_REG(0xED);  
	LCD_WR_DATA(0x64); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0X12); 
	LCD_WR_DATA(0X81); 
	
	LCD_WR_REG(0xE8);  
	LCD_WR_DATA(0x85); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x7A); 
	
	LCD_WR_REG(0xCB);  
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x34); 
	LCD_WR_DATA(0x02); 
	
	LCD_WR_REG(0xF7);  
	LCD_WR_DATA(0x20); 
	
	LCD_WR_REG(0xEA);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	
	LCD_WR_REG(0xC0);    //Power control 
	LCD_WR_DATA(0x1B);   //VRH[5:0] 
	
	LCD_WR_REG(0xC1);    //Power control 
	LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0] 
	
	LCD_WR_REG(0xC5);    //VCM control 
	LCD_WR_DATA(0x30); 	 //3F
	LCD_WR_DATA(0x30); 	 //3C
	
	LCD_WR_REG(0xC7);    //VCM control2 
	LCD_WR_DATA(0XB7); 
	
	LCD_WR_REG(0x36);    // Memory Access Control 
	LCD_WR_DATA(0x08); 
	
	LCD_WR_REG(0x3A);   
	LCD_WR_DATA(0x55); 
	
	LCD_WR_REG(0xB1);   
	LCD_WR_DATA(0x00);   
	LCD_WR_DATA(0x1A); 
	
	LCD_WR_REG(0xB6);    // Display Function Control 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0xA2); 
	
	LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
	LCD_WR_DATA(0x00); 
	
	LCD_WR_REG(0x26);    //Gamma curve selected 
	LCD_WR_DATA(0x01); 
	
	LCD_WR_REG(0xE0);    //Set Gamma 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x2A); 
	LCD_WR_DATA(0x28); 
	LCD_WR_DATA(0x08); 
	LCD_WR_DATA(0x0E); 
	LCD_WR_DATA(0x08); 
	LCD_WR_DATA(0x54); 
	LCD_WR_DATA(0XA9); 
	LCD_WR_DATA(0x43); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 	
	
	LCD_WR_REG(0XE1);    //Set Gamma 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x15); 
	LCD_WR_DATA(0x17); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x11); 
	LCD_WR_DATA(0x06); 
	LCD_WR_DATA(0x2B); 
	LCD_WR_DATA(0x56); 
	LCD_WR_DATA(0x3C); 
	LCD_WR_DATA(0x05); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x3F); 
	LCD_WR_DATA(0x3F); 
	LCD_WR_DATA(0x0F); 
	
	LCD_WR_REG(0x2B); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);
	LCD_WR_REG(0x2A); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);
	
	LCD_WR_REG(0x11); //Exit Sleep
	delay_tft(120);
	LCD_WR_REG(0x29); //display on	
}

void LCD_XYRAM(uint16_t stx,uint16_t sty,uint16_t endx,uint16_t endy)
{
	if( stx>=XNUM || endx>=XNUM || sty>=YNUM || endy>=YNUM )
		return ;
	LCD_WR_REG(XPOSITION);		//列地址  
	LCD_WR_DATA(stx>>8);    
	LCD_WR_DATA(stx&0xff);	//开始列所在位置    	
	LCD_WR_DATA(endx>>8); 
	LCD_WR_DATA(endx&0xff);	//结束列所在位置

	LCD_WR_REG(YPOSITION);  //行地址
	LCD_WR_DATA(sty>>8); 
	LCD_WR_DATA(sty&0xff);	//开始行所在位置
	LCD_WR_DATA(endy>>8); 
	LCD_WR_DATA(endy&0xff);	//开始列所在位置
}

void LCD_Clear(uint16_t color)
{  
	unsigned int i,j;
	
	LCD_XYRAM(0,0,239,319); //复位显示区域
	LCD_WR_REG(0x2C);				//像GRAM中写数据指令
	for(i=0;i<320;i++)
	{
		for(j=0;j<240;j++)
		{
			LCD_WR_DATA(color);
		}
	}
}

void diplayChar(uint16_t color)
{
	unsigned char  i,j,k,t;
	unsigned char c = 0, row=0, line=0;
	i=0;
	t =0;
  LCD_Clear((uint16_t)~0xFF81);
	
	LCD_XYRAM(0,0,15,15);
  LCD_WR_REG(DATAREG);
	
	for(i=0; i<93; )
	{
		for(j=0; j<16; j++)   //一个文字的点阵
		{
			for(c=0; c<8; c++) //一个字节的点阵
			{
				k = (ascii8_16[i][j]<<c) & 0x80; 
				if(k==0x80)
						LCD_WR_DATA(0xFF81);	 //显示区域为白色
				else
						LCD_WR_DATA(~0xFF81); //不显示区域蓝色
			}
		}
	line++;  //显示行已经显示字的个数
	if(line==24)
	{
		line = 0;
		row++;	//行显示满，则换行
	}
	LCD_XYRAM(line*10,row*16,line*10+7,row*16+15);	//跟新设置将要显示的文字点阵区域
	LCD_WR_REG(DATAREG);
	i += 1;
}

LCD_XYRAM((240-16*3)/2,(320-16)/2,(240-16*3)/2+15,(320-16)/2+15);
LCD_WR_REG(DATAREG);
for(i=0;i<3; i++)
{
	for(j=0; j<32; j++)
	{
		for(c=0; c<8; c++)
		{
			k = (my[i][j]<<c) & 0x80; 
			if(k==0x80)
					LCD_WR_DATA(0xFF00);
			else
					LCD_WR_DATA(~0xFF81);
		}
	}
	t++;
	LCD_XYRAM((240-15*3)/2+t*16,(320-15)/2,(240-15*3)/2+t*16+15,(320-16)/2+15);
	LCD_WR_REG(DATAREG);
	}

	
	
//	for(c=0; c<11;)
//	{
//		for(i=0; i<72; i++)
//		for(j=0; j<8; j++)
//		{
//			k = (gbk[i+c*72]<<j)&0x80;
//			if(k==0x80)
//				LCD_WR_DATA(0xFF81);
//			else
//				LCD_WR_DATA(~0xFF81);
//		}
//		c++;
//		LCD_XYRAM(c*24,0,c*24+23,23);
//		LCD_WR_REG(0x2C);
//}

	
}







