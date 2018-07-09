
#ifndef GIMBAL_CAMERA_CONTROL_H
#define GIMBAL_CAMERA_CONTROL_H
  
  #include "usart.h"	
	#include "timers.h"
	
	extern xQueueHandle landingQueueHandle;
	extern TimerHandle_t xTimerUser;
//	unsigned char buff[100] = {0};  //串口发送数据缓冲区，该缓冲区的功能是等待上次DMA发送数据完成之前保存的数据
//	unsigned char saveCount= 0; //已经保存数据的个数
//	unsigned char sendStatus = 0; //串口发送标志  0-忙  1-空闲
 
	#define PINLING_GIMBAL_ZOOM18	 1			//品灵18倍
	#define TUOPU_DOUBLE_CAMERA	 1				//拓扑双相机云台
	
	typedef enum _usart_com{
		COM01=1,
		COM02,
		COM03,
		COM04
	}usart_com;
	
	typedef enum _high_status{
		GIMBAL_PITCH_UP = 0x11,    		//俯仰上下
		GIMBAL_PITCH_DOWN = 0x12,
		GIMBAL_YAW_LEFT = 0x13,	  		//航向左右
		GIMBAL_YAW_RIGHT = 0x14,
		GIMBAL_POSITION = 0x15,					//云台回中
		STOP = 0x16,										//停止动作
		CAMERA_ZOOM = 0x21,					//倍数放大、缩小
		CAMERA_FOCUS = 0x22,				//远近聚焦
		CAMERA_MODE_CHANGE = 0x23,			//模式切换  录像/拍照
		CAMERA_PIP_MODE = 0x24,				//画中画切换
		CAMERA_COLOR_CHANGE = 0x25,					//红外颜色切换
		CAMERA_REC = 0x26,									//开始录像
		CAMERA_TAKE_PHOTO = 0x27,						//拍照
		CAMERA_ZOOM_QUERY = 0x28,						//放大倍数查询
		GIMBAL_PITCH_ANGLE_UP = 0X17,			  //俯仰往上角度
		GIMBAL_PITCH_ANGLE_DOWN = 0X18, 		//俯仰往下角度
		GIMBAL_YAW_ANGLE_LEFT = 0X19,				//航向左角度
		GIMBAL_YAW_ANGLE_DOWN = 0x1A,			//航向右角度
		LANDING_GEAR = 0X31							//起落架升降控制
	}action_code;
		
	
	/*** 品灵云台和相机控制指令 ***/
#if PINLING_GIMBAL_ZOOM18==1    
	
		/*** 云台控制部分 ***/
		#define RATE_MIN	0x00  //最小速度
		#define RATE_MAX  0x3F	//最快速度
		const unsigned char PITCH_UP_RATE[5] = { 0XFF, 0X01, 0X00, 0X08, 0X00 };  //后面需要另加两个自己，一个速度，一个校验和
		const unsigned char PITCH_DOWN_RATE[5] = { 0XFF, 0X01, 0X00, 0X10, 0X00 };
		
		const unsigned char YAW_LEFT_RATE[4] = { 0XFF, 0X01, 0X00, 0X04 };  //后面需另加三个自己，一个速度，一个00，一个校验和
		const unsigned char YAW_RIGHT_RATE[4] = { 0XFF, 0X01, 0X00, 0X02 };
		
		const unsigned char STOP_ACTION[7] = { 0XFF, 0X01, 0X00, 0X00, 0X00, 0X00, 0X01 };  //停止云台动作、变倍停止
		const unsigned char STOP_ACTION_NEW[7] = {0XFF, 0X01, 0X0F, 0X00, 0X00, 0X00, 0X10 }; //新版本
		
		const unsigned char GIMBAL_RESET[7] = { 0XFF, 0X01, 0X10, 0X00, 0X00, 0X00, 0X11 };   //云台回中
		const unsigned char RESET_GIMBAL_POSITION[5] = { 0X3E, 0X1A, 0X00, 0X1A, 0X00 };     //重新手动设置云台的回中位置
		
		const unsigned char FOLLOW_YAW_DISABLE[11] = { 0X3E, 0X1F, 0X06, 0X25, 0X01, 0X1F, 0X00, 0X00, 0X00, 0X00, 0X20 }; //航向锁头模式(云台航向仅受遥控控制)
		const unsigned char FOLLOW_YAW_ENABLE[11] = { 0X3E, 0X1F, 0X06, 0X25, 0X01, 0X1F, 0X01, 0X00, 0X00, 0X00, 0X21 }; //跟随模式，云台跟着飞机转动
		const unsigned char QUERY_FOLLOW_MODE[7] = { 0X3E, 0X40, 0X02, 0X42, 0X01, 0X1F, 0X20 };  //云台锁头模式、跟随模式查询
		/*返回：3E,1F,06,25,01,1F,01,00,00,00,21 -> 跟随模式     3E,1F,06,25,01,1F,01,00,00,00,20 -> 锁头模式*/
		
		const unsigned char ZOOM_DOWN[7] = { 0XFF, 0X01, 0X00, 0X40, 0X00, 0X00, 0X41 };   //相机倍数缩小
		const unsigned char ZOOM_UP[7] = { 0XFF, 0X01, 0X00, 0X20, 0X00, 0X00, 0X21 };     //相机倍数放大
		const unsigned char FOCUS_UP[7] = { 0XFF, 0X01, 0X01, 0X00, 0X00, 0X00, 0X02 };    //远焦
		const unsigned char FOCUS_DOWN[7] = { 0XFF, 0X01, 0X00, 0X80, 0X00, 0X00, 0X81 };  //近焦
		
		const unsigned char IMAGE_REC_MODE_CHANGE[7] = { 0XFF, 0X01, 0X00, 0X07, 0X00, 0X67, 0X6F };  	//录像、拍照模式切换
		const unsigned char IMAGE_RECORD[7] = { 0XFF, 0X01, 0X00, 0X07, 0X00, 0X55, 0X5D };						//录像或者拍照
		const unsigned char QUERY_ZOOM_VALUE[5] = { 0X81, 0X09, 0X04, 0X47, 0XFF };                    //查询相机当前放大倍数
    /* 关于返回相机的放大倍数需要问技术支持 */		
		
		const unsigned char ANGLE_CONTROL_HEADER[4] = { 0X81, 0X01, 0X06, 0X01 };	//角度控制前五个通用字节 后面跟有速度、角度, 另外两个字节和结束字
		
	#endif
	
/*** 拓扑云台和相机控制指令  SMT18040T ***/
	#if TUOPU_DOUBLE_CAMERA==1
		
		#define SMT18040_FRAME_MAX_LEN 27
		#define SMT18040_HEADER_LEN	3
		#define SMT18040_TARGET_LEN	2
		#define SMT18040_PART_LEN	3
		#define SMT18040_FRAME_DATA_LEN  15
		#define SMT18040_FRAME_CRC_LEN	2
		#define CRC_DATA_LEN (SMT18040_HEADER_LEN+SMT18040_TARGET_LEN+SMT18040_PART_LEN+1+1)
		#define SMT18040_READ 'r'
		#define SMT18040_WRITE	'w' 
		void COPY_DATA(char* dest, char* src, unsigned int len)  
		{  while(len--) 
			 *dest++=*src++;  }
		
		typedef struct _SMT18040_frame
		{
			char header[SMT18040_HEADER_LEN];		//帧头3个字节  #TP定长   #tp可变长
			char target[SMT18040_TARGET_LEN];			//目标位 2个字节
			char len;										//数据长度  
			char RW; 		//读或者写'r' 读  'w' 写
			char part[SMT18040_PART_LEN];	//部件
			char data[SMT18040_FRAME_DATA_LEN];	//数据长度	最大15个字节
//			char check[SMT18040_FRAME_CRC_LEN];		//校验位  2个字节
		}camera_frame;
		
		/*** 云台控制部分 ***/
		#define RATE_MIN	0x00  //最小速度
		#define RATE_MAX  0x3F	//最快速度		
	
//		const unsigned char IMAGING_MODE[5] = { 0X81, 0X09, 0X04, 0X47, 0XFF };  //热红外伪彩模式
#endif
static char char_hex(char c)
{
	if(c>=16) return 0;
	return "0123456789ABCDEF"[c];
}
		
/***
*   指定串口发送单个字节
*
***/
void sendByte(usart_com chan, unsigned char c)
{
	switch(chan)
	{
		case COM01:
		{
			usartSendChar(USART1, c);
			break;
		}
		case COM02:
		{
			usartSendChar(USART2, c);
			break;
		}
		case COM03:
		{
			usartSendChar(USART3, c);
			break;
		}
		case COM04:
		{
			break;
		}
		default: break;
	}
}
		
/***
*   指定串口，指令长度发送
*
***/
unsigned char sendControlCode(usart_com chan, const unsigned char *pch, unsigned char len)
{
	unsigned char i = len;
	if( pch==0 && i==0 ) return 0;
	
	for( ; i>0; --i)
	{
		sendByte(chan, *pch);
		++pch;			
	}
	return 1;
}

/***
*   指定数组长度求和   //返回校验和
*
***/
static unsigned char addSum(const unsigned char* pc, unsigned char len)
{
	unsigned int i = 0;
	int sum = 0;
	if(0==pc) return 0;
	
	for( ; i<len; ++i)
	{
		sum += *pc;
		++pc;
	}
	return (unsigned char)sum;
}


/***
*   根据云台转动角度匹配速度 
*
***/
static unsigned char getAngleSpeed(unsigned char angle)
{
	unsigned char speed;
	
	if(angle>150)  return 0;

	if(angle<=30)  //15度以内采用速度15
	{
		speed = 15;
	}else if(angle<=60)  //30度到60度以内采用25
	{
		speed = 25;
	}else if(angle<=90)  //60到90度采用35
	{
		speed = 35;
	}else if(angle<=150) //90到150采用40
	{
		speed = 40;
	}
	return speed;	
}	

/***
*   根据云台转动匹配速度 
*
***/
static unsigned char getSpeed(unsigned char speedSrc)
{
	unsigned char speed;
	
//	if(speedSrc>0x1A)
//	{
//		speed = 0x0F;
//	}else if(speedSrc<=0x1A)
//	{
//		speed = speedSrc;
//	}
	speed = 0x05; 
	return speed;	
}

/***
*   复制数据 
*
***/
void copyCharArray(const unsigned char *src, unsigned char *dest, unsigned int count)
{
	unsigned int i;
	const unsigned char *psrc;
	unsigned char *pdest;
	
	if(0==src || 0==dest || 0==count)  return;
	
	i = count;
	psrc = src;
	pdest = dest;
	
	for( ; i>0; --count)
	{
		*pdest = *psrc;
		++psrc;
		++pdest;
	}	
}

	
/***
*   解析云台控制指令 
*
***/
void gimbal_control_parse(usart_com chan, unsigned char* highCh, unsigned char* low)
{
	static unsigned char recStatus = 1;         //录像判断指令, 1为正在录像， 0为停止录像
	static unsigned char pitchUpStatus = 0;
	static unsigned char pitchDownStatus = 0;
	static unsigned char yawLeftStatus = 0;
	static unsigned char yawRightStatus = 0; 
	
//	if(100==saveCount) saveCount=0;   //保存的数据已经满载，重新开始保存，之前的数据会被覆盖掉
	
	switch(*highCh)
	{
		case	GIMBAL_PITCH_ANGLE_UP:			//俯仰往上角度
		{
			
			sendControlCode(chan, &ANGLE_CONTROL_HEADER[0], sizeof(ANGLE_CONTROL_HEADER)/sizeof(ANGLE_CONTROL_HEADER[0]));
			sendByte( chan, getAngleSpeed((*low) ));  //速度
			sendByte( chan, *low );   //角度
			sendByte(chan, 0x03);
			sendByte(chan, 0x01);
			sendByte(chan, 0xff);
			break;
		}
		case GIMBAL_PITCH_ANGLE_DOWN:  		//俯仰往下角度
		{
			sendControlCode(chan, &ANGLE_CONTROL_HEADER[0], sizeof(ANGLE_CONTROL_HEADER)/sizeof(ANGLE_CONTROL_HEADER[0]));
			sendByte( chan, getAngleSpeed((*low)));  //速度
			sendByte( chan, *low );   //角度
			sendByte(chan, 0x03);
			sendByte(chan, 0x02);
			sendByte(chan, 0xff);
			break;
		}
		case GIMBAL_YAW_ANGLE_LEFT: 				//航向左角度
		{
			sendControlCode(chan, &ANGLE_CONTROL_HEADER[0], sizeof(ANGLE_CONTROL_HEADER)/sizeof(ANGLE_CONTROL_HEADER[0]));
			sendByte( chan, getAngleSpeed((*low)) );  //速度
			sendByte( chan, *low );   //角度
			sendByte(chan, 0x01);
			sendByte(chan, 0x03);
			sendByte(chan, 0xff);
			break;
		}
		case GIMBAL_YAW_ANGLE_DOWN: 		//航向右角度
		{
			sendControlCode(chan, &ANGLE_CONTROL_HEADER[0], sizeof(ANGLE_CONTROL_HEADER)/sizeof(ANGLE_CONTROL_HEADER[0]));
			sendByte( chan, getAngleSpeed((*low)) );  //速度
			sendByte( chan, *low );   //角度
			sendByte(chan, 0x02);
			sendByte(chan, 0x03);
			sendByte(chan, 0xff);
			break;
		}
		case LANDING_GEAR:                //起落架升降  
		{
			if(*low==0x01)									//升
			{
				IWDG_Feed();
//				xSemaphoreGive(xSemaphore_landing_gear);
//				xQueueSendToBack(landingQueueHandle, low, 10/portMAX_DELAY);
			}else if(*low==0x02)						//降
			{
				IWDG_Feed();
//				xSemaphoreGive(xSemaphore_landing_gear);
//				xQueueSendToBack(landingQueueHandle, low, 10/portMAX_DELAY);
			}else
			{
				
			}
			break;
		}
		
		case GIMBAL_PITCH_UP:    		//俯仰上下
		{
			pitchUpStatus = 1;
			sendControlCode(chan, &PITCH_UP_RATE[0], sizeof(PITCH_UP_RATE)/sizeof(PITCH_UP_RATE[0]));
			sendByte( chan, getSpeed(*low) );
			sendByte( chan, addSum(&PITCH_UP_RATE[1], 4) + getSpeed(*low));		
			break;
		}
		case GIMBAL_PITCH_DOWN:
		{
			pitchDownStatus = 1;
			sendControlCode(chan, &PITCH_DOWN_RATE[0], sizeof(PITCH_DOWN_RATE)/sizeof(PITCH_DOWN_RATE[0]));
			sendByte( chan, getSpeed(*low) );
			sendByte( chan, addSum(&PITCH_DOWN_RATE[1], 4) + getSpeed(*low));

			break;
		}
		case GIMBAL_YAW_LEFT:	  		//航向左右
		{
			yawLeftStatus = 1;

			sendControlCode(chan, &YAW_LEFT_RATE[0], sizeof(YAW_LEFT_RATE)/sizeof(YAW_LEFT_RATE[0]));
			sendByte( chan, getSpeed(*low));
			sendByte( chan, 0);
			sendByte( chan, addSum(&YAW_LEFT_RATE[1], 3) + getSpeed(*low) );

			break;
		}
		case GIMBAL_YAW_RIGHT:
		{
			yawRightStatus = 1;
			if(yawRightStatus<=4)
			{				
				sendControlCode(chan, &YAW_RIGHT_RATE[0], sizeof(YAW_RIGHT_RATE)/sizeof(YAW_RIGHT_RATE[0]));
				sendByte( chan, getSpeed(*low) );
				sendByte( chan, 0);
				sendByte( chan, addSum(&YAW_RIGHT_RATE[1], 3) + getSpeed(*low) );
			}
			break;
		}
		case GIMBAL_POSITION:					
		{
			if(*low==1)    					//云台回中
			{
				sendControlCode(chan, &GIMBAL_RESET[0], sizeof(GIMBAL_RESET)/sizeof(GIMBAL_RESET[0]));
			}else if(*low==2)				//手动设置回中位置
			{
				sendControlCode(chan, &RESET_GIMBAL_POSITION[0], sizeof(RESET_GIMBAL_POSITION)/sizeof(RESET_GIMBAL_POSITION[0]));
			}else if(*low==3)				//设置为锁头模式
			{
				sendControlCode(chan, &FOLLOW_YAW_DISABLE[0], sizeof(FOLLOW_YAW_DISABLE)/sizeof(FOLLOW_YAW_DISABLE[0]));										
			}else if(*low==4)				//设置为航向模式
			{
				sendControlCode(chan, &FOLLOW_YAW_ENABLE[0], sizeof(FOLLOW_YAW_ENABLE)/sizeof(FOLLOW_YAW_ENABLE[0]));
			}else if(*low==5)				//云台模式查询			
			{
				sendControlCode(chan, &QUERY_FOLLOW_MODE[0], sizeof(QUERY_FOLLOW_MODE)/sizeof(QUERY_FOLLOW_MODE[0]));
			}else
		  {
				
			}				
			break;
		}
		case STOP:										//停止动作
		{
			if(*low==1)
			{
//				sendControlCode(chan, &STOP_ACTION[0], sizeof(STOP_ACTION)/sizeof(STOP_ACTION[0]));
//				xTimerStop( xTimerUser, 100 );
				xTimerStart( xTimerUser, 100 );
				pitchUpStatus = 0;
				pitchDownStatus = 0;
				yawLeftStatus = 0;
				yawRightStatus = 0; 
			}
			break;
		}
	  case CAMERA_ZOOM:					//倍数放大、缩小
		{
			if(*low==1)							//倍数缩小
			{
				sendControlCode(chan, &ZOOM_DOWN[0], sizeof(ZOOM_DOWN)/sizeof(ZOOM_DOWN[0]));
			}else if(*low==2)				//倍数放大
			{
				sendControlCode(chan, &ZOOM_UP[0], sizeof(ZOOM_UP)/sizeof(ZOOM_UP[0]));
			}else{
			
			}
			break;
		}
		case CAMERA_FOCUS:				//远近聚焦
		{
			if(*low==1)							//近
			{
				sendControlCode(chan, &FOCUS_DOWN[0], sizeof(FOCUS_DOWN)/sizeof(FOCUS_DOWN[0]));
			}else if(*low==2)				//远
			{
				sendControlCode(chan, &FOCUS_UP[0], sizeof(FOCUS_UP)/sizeof(FOCUS_UP[0]));
			}else{
			
			}
			break;
		}

		case CAMERA_MODE_CHANGE:			//模式切换  录像/拍照
		{
			if(*low==1)
			{
				sendControlCode(chan, &IMAGE_REC_MODE_CHANGE[0], sizeof(IMAGE_REC_MODE_CHANGE)/sizeof(IMAGE_REC_MODE_CHANGE[0]));
			}
			break;
		}
		case CAMERA_PIP_MODE:				//画中画切换
		{
			if(*low==1)
			{
				sendControlCode(chan, &IMAGE_REC_MODE_CHANGE[0], sizeof(IMAGE_REC_MODE_CHANGE)/sizeof(IMAGE_REC_MODE_CHANGE[0]));
			}
			break;
		}
		case CAMERA_COLOR_CHANGE:		//红外颜色切换
		{
			if(*low==1)
			{
				//sendControlCode(chan, &IMAGING_MODE[0], sizeof(IMAGING_MODE)/sizeof(IMAGING_MODE[0]));
				sendControlCode(chan, &QUERY_ZOOM_VALUE[0], sizeof(QUERY_ZOOM_VALUE)/sizeof(QUERY_ZOOM_VALUE[0]));
			}
			break;
		}
		case CAMERA_REC:				//开始录像
		{
			if(*low==1)						
			{
				
			}else if(*low==2&&recStatus==1)		 //开始录像		
			{
				recStatus = 0;
				sendControlCode(chan, &IMAGE_RECORD[0], sizeof(IMAGE_RECORD)/sizeof(IMAGE_RECORD[0]));
			}else if(*low==3&&recStatus==0)		 //停止录像
			{
				recStatus = 1;
				sendControlCode(chan, &IMAGE_RECORD[0], sizeof(IMAGE_RECORD)/sizeof(IMAGE_RECORD[0]));
			}else
			{
			
			}
			break;
		}
		case CAMERA_TAKE_PHOTO:			//拍照
		{
			if(*low==2)
			{
				sendControlCode(chan, &IMAGE_RECORD[0], sizeof(IMAGE_RECORD)/sizeof(IMAGE_RECORD[0]));
			}
			break;
		}
		case CAMERA_ZOOM_QUERY:			//放大倍数查询
		{
			if(*low==1)
			{
				sendControlCode(chan, &QUERY_ZOOM_VALUE[0], sizeof(QUERY_ZOOM_VALUE)/sizeof(QUERY_ZOOM_VALUE[0]));
			}
			break;
		}
		default: break;
	}
}
//end function

/* 双相机云台控制指令 */
void double_send_code(usart_com chan, char* header, char* target, char len,  char r_w, char* part, char* data)
{
	char frame[SMT18040_FRAME_MAX_LEN];
	char* pc = frame;
	unsigned int crc = 0;
	unsigned char crc_len =0;
	if(0==header || 0==target) return;
	COPY_DATA(pc,header,SMT18040_HEADER_LEN);  //复制帧头
	pc += SMT18040_HEADER_LEN;
	
	COPY_DATA(pc,target,SMT18040_TARGET_LEN);  //复制目标
	pc += SMT18040_TARGET_LEN;
	
	*pc = char_hex(len);
	++pc;
	
	*pc = r_w;
	++pc;
	
	COPY_DATA(pc,part,SMT18040_PART_LEN); //标识
	pc += SMT18040_PART_LEN;
		
	COPY_DATA(pc, data, len);//数据
	pc += len;
	
	for(crc_len=0; crc_len<CRC_DATA_LEN+len; ++crc_len)
	{
		crc += frame[crc_len];
	}
	*pc++ = char_hex(crc>>4 & 0x0f);
	*pc = char_hex(crc & 0x0f);
//	sendControlCode(chan, frame, CRC_DATA_LEN+len+SMT18040_FRAME_CRC_LEN);   //向串口发送数据
	insertDataToBuff(chan, frame, CRC_DATA_LEN+len+SMT18040_FRAME_CRC_LEN);
}

/***
*  双光相机云台控制
*
***/
void double_camera_control_parse(usart_com chan, unsigned char* highCh, unsigned char* low)
{
	static unsigned char recStatus = 1;         //录像判断指令, 1为正在录像， 0为停止录像
	static unsigned char pitchUpStatus = 0;
	static unsigned char pitchDownStatus = 0;
	static unsigned char yawLeftStatus = 0;
	static unsigned char yawRightStatus = 0; 
	switch(*highCh)
	{
		case	GIMBAL_PITCH_ANGLE_UP:			//俯仰往上角度
		{
//			sendControlCode(chan, &ANGLE_CONTROL_HEADER[0], sizeof(ANGLE_CONTROL_HEADER)/sizeof(ANGLE_CONTROL_HEADER[0]));
//			sendByte( chan, getAngleSpeed((*low) ));  //速度
//			sendByte( chan, *low );   //角度
//			sendByte(chan, 0x03);
//			sendByte(chan, 0x01);
//			sendByte(chan, 0xff);
			break;
		}
		case GIMBAL_PITCH_ANGLE_DOWN:  		//俯仰往下角度
		{
//			sendControlCode(chan, &ANGLE_CONTROL_HEADER[0], sizeof(ANGLE_CONTROL_HEADER)/sizeof(ANGLE_CONTROL_HEADER[0]));
//			sendByte( chan, getAngleSpeed((*low)));  //速度
//			sendByte( chan, *low );   //角度
//			sendByte(chan, 0x03);
//			sendByte(chan, 0x02);
//			sendByte(chan, 0xff);
			break;
		}
		case GIMBAL_YAW_ANGLE_LEFT: 				//航向左角度
		{
//			sendControlCode(chan, &ANGLE_CONTROL_HEADER[0], sizeof(ANGLE_CONTROL_HEADER)/sizeof(ANGLE_CONTROL_HEADER[0]));
//			sendByte( chan, getAngleSpeed((*low)) );  //速度
//			sendByte( chan, *low );   //角度
//			sendByte(chan, 0x01);
//			sendByte(chan, 0x03);
//			sendByte(chan, 0xff);
			break;
		}
		case GIMBAL_YAW_ANGLE_DOWN: 		//航向右角度
		{
//			sendControlCode(chan, &ANGLE_CONTROL_HEADER[0], sizeof(ANGLE_CONTROL_HEADER)/sizeof(ANGLE_CONTROL_HEADER[0]));
//			sendByte( chan, getAngleSpeed((*low)) );  //速度
//			sendByte( chan, *low );   //角度
//			sendByte(chan, 0x02);
//			sendByte(chan, 0x03);
//			sendByte(chan, 0xff);
			break;
		}
		case LANDING_GEAR:                //起落架升降  
		{
			if(*low==0x01)									//升
			{
//				xSemaphoreGive(xSemaphore_landing_gear);
//				xQueueSendToBack(landingQueueHandle, low, 10/portMAX_DELAY);
			}else if(*low==0x02)						//降
			{
//				xSemaphoreGive(xSemaphore_landing_gear);
//				xQueueSendToBack(landingQueueHandle, low, 10/portMAX_DELAY);
			}else
			{
				
			}
			break;
		}
		
		case GIMBAL_PITCH_UP:    		//俯仰上下
		{
			++pitchUpStatus;
			if(pitchUpStatus<=4)
			{
				double_send_code(chan,"#TP", "UP", 2, SMT18040_WRITE, "PTZ", "01");
			}			
			break;
		}
		case GIMBAL_PITCH_DOWN:
		{
			++pitchDownStatus;
			if(pitchDownStatus<=4)
			{
				double_send_code(chan,"#TP", "UP", 2, SMT18040_WRITE, "PTZ", "02");
			}
			break;
		}
		case GIMBAL_YAW_LEFT:	  		//航向左右
		{
			++yawLeftStatus;
			if(yawLeftStatus<=4)
			{
				double_send_code(chan,"#TP", "UP", 2, SMT18040_WRITE, "PTZ", "03");
			}
			break;
		}
		case GIMBAL_YAW_RIGHT:
		{
			++yawRightStatus;
			if(yawRightStatus<=4)
			{			
					double_send_code(chan,"#TP", "UP", 2, SMT18040_WRITE, "PTZ", "04");				
			}
			break;
		}
		case GIMBAL_POSITION:					
		{
			if(*low==1)    					//云台回中
			{
				double_send_code(chan,"#TP", "UP", 2, SMT18040_WRITE, "PTZ", "05");
			}else if(*low==2)				//手动设置回中位置
			{
//				sendControlCode(chan, &RESET_GIMBAL_POSITION[0], sizeof(RESET_GIMBAL_POSITION)/sizeof(RESET_GIMBAL_POSITION[0]));
			}else if(*low==3)				//设置为锁头模式
			{
				double_send_code(chan,"#TP", "UP", 2, SMT18040_WRITE, "PTZ", "07");									
			}else if(*low==4)				//设置为航向模式
			{
				double_send_code(chan,"#TP", "UP", 2, SMT18040_WRITE, "PTZ", "06");;
			}else if(*low==5)				//云台模式查询			
			{
//				sendControlCode(chan, &QUERY_FOLLOW_MODE[0], sizeof(QUERY_FOLLOW_MODE)/sizeof(QUERY_FOLLOW_MODE[0]));
			}else
		  {
				
			}				
			break;
		}
		case STOP:										//停止动作
		{
			if(*low==1)
			{
				double_send_code(chan,"#TP", "UP", 2, SMT18040_WRITE, "PTZ", "00");  //停止云台俯仰
				double_send_code(chan,"#tp", "UM", 8, SMT18040_WRITE, "HIT", ":WFCBBFE66");  //聚焦、变倍停止
				pitchUpStatus = 0;
				pitchDownStatus = 0;
				yawLeftStatus = 0;
				yawRightStatus = 0; 
			}
			break;
		}
	  case CAMERA_ZOOM:					//倍数放大、缩小
		{
			if(*low==1)							//倍数缩小
			{
				double_send_code(chan,"#tp", "UM", 8, SMT18040_WRITE, "HIT", ":WFCBB9B56");
			}else if(*low==2)				//倍数放大
			{
				double_send_code(chan,"#tp", "UM", 8, SMT18040_WRITE, "HIT", ":WFCBB994D");
			}else{
			
			}
			break;
		}
		case CAMERA_FOCUS:				//远近聚焦
		{
			if(*low==1)							//近
			{
				double_send_code(chan,"#tp", "UM", 8, SMT18040_WRITE, "HIT", ":WFCBBAA5D");
			}else if(*low==2)				//远
			{
				double_send_code(chan,"#tp", "UM", 8, SMT18040_WRITE, "HIT", ":WFCBBA955");
			}else{
			
			}
			break;
		}

		case CAMERA_MODE_CHANGE:			//模式切换  录像/拍照
		{
			if(*low==1)
			{
				static unsigned char pipMode = 0;
				char data[2];
				if(pipMode>7)	pipMode = 0;
				data[0] = char_hex(0);
				data[1] = char_hex(pipMode&0x0f);
				double_send_code(chan,"#TP", "UD", 2, SMT18040_WRITE, "AWB", &data[0]);
				double_send_code(chan,"#TP", "UD", 2, SMT18040_WRITE, "EVS", &data[0]);
				++pipMode;
//				sendControlCode(chan, &IMAGE_REC_MODE_CHANGE[0], sizeof(IMAGE_REC_MODE_CHANGE)/sizeof(IMAGE_REC_MODE_CHANGE[0]));
			}
			break;
		}
		case CAMERA_PIP_MODE:				//画中画切换
		{
			if(*low==1)
			{
				static unsigned char pipMode = 0;
				char data[2];
				if(pipMode>3)	pipMode = 0;
				data[0] = char_hex(0);
				data[1] = char_hex(pipMode&0x0f);
				double_send_code(chan,"#TP", "UD", 2, SMT18040_WRITE, "PIP", &data[0]);
				++pipMode;
			}
			break;
		}
		case CAMERA_COLOR_CHANGE:		//红外颜色切换
		{
			if(*low==1)
			{
				static unsigned char colorMode = 0;
				char data[2];
				if(colorMode>0x09)	colorMode = 0;
				data[0] = char_hex(colorMode>>4&0x0f);
				data[1] = char_hex(colorMode&0x0f);
				double_send_code(chan,"#TP", "UE", 2, SMT18040_WRITE, "IMG", &data[0]);
//				double_send_code(chan,"#TP", "UE", 2, SMT18040_WRITE, "DZM", &data[0]);
				++colorMode;
			}
			break;
		}
		case CAMERA_REC:				//开始录像
		{
			if(*low==1)						
			{
				
			}else if(*low==2&&recStatus==1)		 //开始录像		
			{
				recStatus = 0;
				double_send_code(chan,"#TP", "UD", 2, SMT18040_WRITE, "REC", "11");
			}else if(*low==3&&recStatus==0)		 //停止录像
			{
				recStatus = 1;
				double_send_code(chan,"#TP", "UD", 2, SMT18040_WRITE, "REC", "00");
			}else
			{
			
			} 
			break;
		}
		case CAMERA_TAKE_PHOTO:			//拍照
		{
			if(*low==2)
			{
				double_send_code(chan,"#TP", "UD", 2, SMT18040_WRITE, "CAP", "11");
				double_send_code(chan,"#TP", "UD", 2, SMT18040_WRITE, "CAP", "NN");
//				sendControlCode(chan, &IMAGE_RECORD[0], sizeof(IMAGE_RECORD)/sizeof(IMAGE_RECORD[0]));
			}
			break;
		}
		case CAMERA_ZOOM_QUERY:			//放大倍数查询
		{
			if(*low==1)
			{
//				sendControlCode(chan, &QUERY_ZOOM_VALUE[0], sizeof(QUERY_ZOOM_VALUE)/sizeof(QUERY_ZOOM_VALUE[0]));
			}
			break;
		}
		default: break;
	}
}		
//end function

#endif
