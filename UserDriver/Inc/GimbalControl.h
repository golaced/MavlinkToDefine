
#ifndef GIMALBAL_CONTROL_H
#define GIMALBAL_CONTROL_H

#define uchar unsigned char 

#define GROUND_STATION_CODE_ID  169  

#define GIMAL_CONTROL_STX 0xFF     //控制帧起始位


/***	
*	 云台控制数据帧结构
*
*/
typedef struct __gimbal_control_message {
	
	uchar stx;						//起始数据帧
	uchar general1;				//通用参数1
	uchar general2; 			//通用参数2
	uchar general3;		//动作指令代码	
	uchar action_param1;	//动作参数1	
	uchar action_param2;	//动作参数2
  uchar add_result;			//第二个参数到第六个参数的数据和
	
} gimbal_control_message_t;

/***	
*	 云台控制串口通道选择
*
*/
typedef enum {
	GIMBAL_CONTROL_COMM_1,
	GIMBAL_CONTROL_COMM_2,
	GIMBAL_CONTROL_COMM_3,
} gimbal_control_channel_t;

/***	
*	 数据解析状态标志
*
*/
typedef enum {
	GIMBAL_PARSE_STATE_IDLE,
	GIMBAL_PARSE_STATE_GOT_STX,
	GIMBAL_PARSE_STATE_GOT_GENERAL1,
	GIMBAL_PARSE_STATE_GOT_GENERAL2,
} gimbal_control_parse_state_t;///< The state machine for the comm parser

/***	
*	 解析完成状态标志
*
*/
typedef enum {
	GIMBAL_CONTROL_FRAMING_INCOMPLETE=0,   //解析中
	GIMBAL_CONTROL_FRAMING_OK=1,						//解析成功
	GIMBAL_CONTROL_FRAMING_OK1=2,						//解析成功
	GIMBAL_CONTROL_FRAMING_BAD=3,					//解析错误
} gimbal_control_framing_t;

/***	
*	 识别到的动作指令
*
*/
//typedef enum {
//	ZOOM_DOWN=0,
//	ZOOM_UP,
//	FOCUS_DOWM,
//	FOCUS_UP,
//	ZOOM_FOCUS_STOP,
//	REC_REVER,
//	DIP_MODE,
//	INFRARED_IMAGE_MODE,
//	PITCH_UP,
//	PITCH_DOWN,
//	YAW_LEFT,
//	YAW_RIGHT,
//	GIMALBAL_STOP,
//	GIMALBAL_RETURN，
//	NONE
//} gimbal_control_action;

static uchar checksum(uchar uc, uchar clear);
gimbal_control_framing_t gimbal_frame_char_buffer(uchar *buff, gimbal_control_parse_state_t* parse_state,  uchar c);

#endif
