
#include "GimbalControl.h"

static uchar checksum(uchar uc, uchar clear)
{	
	static uchar sum = 0;
	if(0==clear) 
	{
		sum = 0;
		return 0;
	}
	sum += uc;
	return sum;
}

gimbal_control_framing_t gimbal_frame_char_buffer(uchar *buff, gimbal_control_parse_state_t* parse_state,  uchar c)
{
	switch (*parse_state)
		{
			case GIMBAL_PARSE_STATE_IDLE:
			{
				if (c == GIMAL_CONTROL_STX || c == 0x81)	//起始位
				{
					*parse_state = GIMBAL_PARSE_STATE_GOT_STX;  //已经识别起始位
					*buff = c;
					checksum(0,0);
				}
				break;
			}

			case GIMBAL_PARSE_STATE_GOT_STX:             //起始位后一位数据
			{
				if(c==0x01)
				{
					*parse_state = GIMBAL_PARSE_STATE_GOT_GENERAL1;
					*(buff+1) = c;
					checksum(c, 1);
				}
				else if(c==0x09)
				{
					*parse_state = GIMBAL_PARSE_STATE_GOT_GENERAL2;
					*(buff+1) = c;
				}
				else
				{
					*parse_state = GIMBAL_PARSE_STATE_IDLE;
					checksum(0, 0);
				}
				break;
			}
			
			case GIMBAL_PARSE_STATE_GOT_GENERAL1:
			{
				static uchar valueCount = 2;
				static uchar sum = 0;
				if(valueCount>=6)
				{
					valueCount = 2;
					*(buff+6)  = c;
					*parse_state = GIMBAL_PARSE_STATE_IDLE;
					if(sum==c)
					{
						return  GIMBAL_CONTROL_FRAMING_OK;
					}
					else
					{
						return GIMBAL_CONTROL_FRAMING_BAD;
					}
				}
				if(valueCount<=5)
				{
					*(buff+valueCount) = c;
					++valueCount;
					sum = checksum(c, 1);
				}
				break;
			}		

			case GIMBAL_PARSE_STATE_GOT_GENERAL2:
			{
				static uchar valueCount = 2;
				*(buff+valueCount) = c;
				++valueCount;
				if(valueCount>=5)
				{
					valueCount = 2;
					*parse_state = GIMBAL_PARSE_STATE_IDLE;
					if(c==0xff)
					{
						return  GIMBAL_CONTROL_FRAMING_OK1;
					}
					else
					{
						return GIMBAL_CONTROL_FRAMING_BAD;
					}
				}
				break;
			}
//		case GIMALBAL_PARSE_STATE_GOT_GENERAL1:
//		{
//			if(c==0x00)
//			{
//				parse_state = GIMALBAL_PARSE_STATE_GOT_GENERAL2;		//第三个数据为00
//				checksum(c, 1);
//			}
//			else if(c==0x10)
//			{
//				*action = GIMALBAL_RETURN;   //云台回中指令
//				parse_state = GIMALBAL_PARSE_STATE_GOT_GENERAL2;		//第三个数据为00
//				checksum(c, 1);				
//			}
//			else if(c==0x01)
//			{
//				*action = FOCUS_UP;         //远焦
//				parse_state = GIMALBAL_PARSE_STATE_GOT_GENERAL2;		//第三个数据为00
//				checksum(c, 1);				
//			} else
//			{
//				*action = NONE;  
//				parse_state = GIMALBAL_PARSE_STATE_IDLE;
//				checksum(c, 0);
//			}
//			rxmsg->general2 = c;
//			break;
//		}
//		
//		case GIMALBAL_PARSE_STATE_GOT_GENERAL2:
//		{
//			switch(c)
//			{
//				parse_state = GIMALBAL_PARSE_STATE_GOT_GENERAL3;
//				checksum(c, 1);
//				case 0x40:			//缩小
//				{
//					*action = ZOOM_DOWN;
//					break;
//				}
//				case 0x20:
//				{
//					*action = ZOOM_UP;
//					break;
//				}
//				case 0x00:
//				{
//					*action = ZOOM_FOCUS_STOP;
//					break;
//				}
//				case 0x80:
//				case 0x07:
//				case 0x08:
//				case 0x10:
//				case 0x04:
//				case 0x02:
//				default: 
//				{
//					parse_state = GIMALBAL_PARSE_STATE_IDLE;
//					checksum(0,0);
//					break;
//				}
//					
//			}
//			break;
//		}
	}
	return GIMBAL_CONTROL_FRAMING_INCOMPLETE;
}
