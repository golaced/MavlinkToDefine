
#include "user_tasks.h"
#include "mavlink.h"
#include <stdio.h>
#include "led.h"
#include "joylink.h"

#define AWESOME_DEBUG 0

const unsigned char init_msg_frame[] = { 	0x73, 0x68, 0x20, 0x2F, 0x65, 0x74, 0x63, 0x2F, 0x69, 0x6E, 0x69, 0x74, 0x2E, 0x64, 0x2F, 0x72, 0x63, 0x2E, 0x75, 0x73, 0x62, 0x0A};
//														0xFA, 0x06, 0x00, 0xFE, 0xBD, 0x42, 0x02, 0x00, 0x00, 0x00, 0x02, 0x01, 0x1D, 0x88,\
//														0xFA, 0x06, 0x01, 0xFE, 0xBD, 0x42, 0x02, 0x00, 0x00, 0x00, 0x02, 0x01, 0x8C, 0xDD,\
//														0xFA, 0x06, 0x02, 0xFE, 0xBD, 0x42, 0x02, 0x00, 0x00, 0x00, 0x06, 0x01, 0x5E, 0x40,\
//														0xFA, 0x06, 0x03, 0xFE, 0xBD, 0x42, 0x02, 0x00, 0x00, 0x00, 0x06, 0x01, 0xCF, 0x15,\
//														0xFA, 0x06, 0x04, 0xFE, 0xBD, 0x42, 0x04, 0x00, 0x00, 0x00, 0x0A, 0x01, 0x47, 0x48,\
//														0xFA, 0x06, 0x05, 0xFE, 0xBD, 0x42, 0x04, 0x00, 0x00, 0x00, 0x0A, 0x01, 0xD6, 0x1D,\
//														0xFA, 0x06, 0x06, 0xFE, 0xBD, 0x42, 0x04, 0x00, 0x00, 0x00, 0x0B, 0x01, 0xB9, 0xB9,\
//														0xFA ,0x06, 0x07, 0xFE, 0xBD, 0x42, 0x04, 0x00, 0x00, 0x00, 0x0B, 0x01, 0x28, 0xEC,\
//														0xFA, 0x06, 0x08, 0xFE, 0xBD, 0x42, 0x02, 0x00, 0x00, 0x00, 0x0C, 0x01, 0xAC, 0x24,\
//														0xFA, 0x06, 0x09, 0xFE, 0xBD, 0x42, 0x02, 0x00, 0x00, 0x00, 0x0C, 0x01, 0x3D, 0x71,\
//														0xFA, 0x06, 0x0A, 0xFE, 0xBD, 0x42, 0x02, 0x00, 0x00, 0x00, 0x01, 0x01, 0xF1, 0x70,\
//														0xFA, 0x06, 0x0B, 0xFE, 0xBD, 0x42, 0x02, 0x00, 0x00, 0x00, 0x01, 0x01, 0x60, 0x25,\
//														0xFA, 0x06, 0x0C, 0xFE, 0xBD, 0x42, 0x02, 0x00, 0x00, 0x00, 0x03, 0x01, 0x3E, 0x30,\
//														0xFA, 0x06, 0x0D, 0xFE, 0xBD, 0x42, 0x02, 0x00, 0x00, 0x00, 0x03, 0x01, 0xAF, 0x65
//};

SemaphoreHandle_t xSemaphore_landing_gear = NULL; 
SemaphoreHandle_t xSemaphore_handle_gear = NULL; 	

/*** 数据队列定义 ***/
#define COM1_RX_QUEUE_LEN  100   /* 队列的深度，字节单位 */
#define COM1_TX_QUEUE_LEN  100
#define COM2_RX_QUEUE_LEN  (2048+126) 
#define COM2_TX_QUEUE_LEN  100
#define COM3_RX_QUEUE_LEN  (1024)
#define COM3_TX_QUEUE_LEN  100
#define MAVLINK_PACK_ARRAY_LEN 5

xQueueHandle com1_tx_queue_handle = NULL; /*创建队列句柄*/
xQueueHandle com1_rx_queue_handle = NULL;
xQueueHandle com2_rx_queue_handle = NULL;
xQueueHandle com2_tx_queue_handle = NULL;
xQueueHandle com3_rx_queue_handle = NULL;
xQueueHandle com3_tx_queue_handle = NULL;
/***  end  ***/

TimerHandle_t xTimerUser;   //软件定时器 

/* mavlink协议系统sysid、compid定义 */
extern mavlink_system_t gcs_mavlink_system;
mavlink_system_t drone_mavlink_system;
#define BOARD_DEV_ID  0xBD
#define BOARD_SYS_ID  0xFE
mavlink_system_t board_mavlink_system = { 0xFE, 0xBD };
unsigned char mav_mission_up_flag = 0;  /* 上传航点信息标志位，如果正在上传，则为1，其他信息不通过串口发送 */ 
TickType_t mav_mission_up_start_time = 0;

#if AWESOME_DEBUG
	typedef struct _mission_value
	{
		uint16_t mission_count;
		uint16_t mission_seq;
		unsigned char read_sucess;
		joylink_mission_element mission_ele;
	} mission_value;

	mission_value mission;
	
	mavlink_mission_item_t mavlink_mission_item[10];
	
#endif



/***  end  ***/
	
void AppStart(void)
{
	com2_rx_queue_handle = xQueueCreate(COM2_RX_QUEUE_LEN, sizeof(char));
	com3_rx_queue_handle = xQueueCreate(COM3_RX_QUEUE_LEN, sizeof(char));
	
	if(com3_rx_queue_handle!=NULL)
	{
		xTaskCreate(usart3_receive_task, USART3_TASK_NAME, 1024, 0, 2, 0);
	}
	
	if(com2_rx_queue_handle!=NULL)
	{
		xTaskCreate(usart2_receive_task, USART2_TASK_NAME, 1024, 0, 1, 0);
	}
	
	xTaskCreate(feed_dog_task, FEED_DOG_TASK_NAME, 50, 0, 0, 0);  
	xTaskCreate(LED_Task, LED_TASK, 32, 0, 0, 0); 
		
	taskEXIT_CRITICAL();
	taskENABLE_INTERRUPTS();

	USART_Cmd(USART2, ENABLE);	
//	USART_Cmd(USART1, ENABLE);
	USART_Cmd(USART3, ENABLE);
//	usartSendString(USART1, "usart1 test...\r\n");
	usartSendString(USART3, "Board: STM Board_V1.4\r\n");
	usartSendString(USART3, JOYLINK_VERSION);
	usartSendString(USART3, "\r\n");
	
	vTaskStartScheduler();
	while(1);
}

void feed_dog_task(void *pvParameters)
{
	while(1)
	{
		vTaskDelay( 300/portTICK_RATE_MS  );
		IWDG_Feed();
	}
}

///***	LED1闪烁任务 ***/
void LED_Task(void *pvParameters)
{	
	while(1)
	{	
		LED_Action(ON);
		IWDG_Feed();
		vTaskDelay(getFlashingTime() / portTICK_PERIOD_MS);
		LED_Action(OFF);
		IWDG_Feed();
		vTaskDelay(getFlashingTime() / portTICK_PERIOD_MS);
		
	}
}

/*** USART2 串口数据处理任务 接飞控串口 ***/
void usart2_receive_task(void *pvParameters)
{	
	mavlink_status_t r_mavlink_status;    //解析成功后的数据帧状态标志位
	mavlink_message_t r_message;					//解析成功的数据帧
	mavlink_framing_t parse_result = MAVLINK_FRAMING_INCOMPLETE;    //解析函数返回的结果
//	mavlink_message_t mavlink_msg;
		
	char ch = 0;  /* 队列接收数据单元 */

	while(1)
	{		
		if(pdPASS==xQueueReceive(com2_rx_queue_handle, &ch, 50/portTICK_RATE_MS)) 
		{				
				/* 0，1，2分别代表串口通道1、2、3*/
				parse_result = (mavlink_framing_t)mavlink_parse_char(MAVLINK_COMM_2, ch, &r_message, &r_mavlink_status);  //解析一个字符
				
				if(MAVLINK_FRAMING_OK==parse_result)    //成功解析一帧数据
				{
					drone_mavlink_system.sysid = r_message.sysid;
					drone_mavlink_system.compid = r_message.compid;
					
					if(xTaskGetTickCount()-mav_mission_up_start_time>=1000)
					{
						mav_mission_up_flag = 0;
					}
					
					switch(r_message.msgid)
					{
						/* 心跳包 */
						case MAVLINK_MSG_ID_HEARTBEAT:     //心跳包  #0   ok
						{
							static unsigned char heart_count= 0;
							static unsigned char request_flag = 0;
							joylink_heartbeat heart_beat;
						
							heart_beat.type = mavlink_msg_heartbeat_get_type(&r_message);
							heart_beat.flight_mode = mavlink_msg_heartbeat_get_custom_mode(&r_message);
							heart_beat.system_status = mavlink_msg_heartbeat_get_system_status(&r_message);		
							heart_beat.base_mode = mavlink_msg_heartbeat_get_base_mode(&r_message);	

							++heart_count;
							if(heart_count>3&&0==request_flag)
							{
								request_flag = 1;
								request_send_data();
							}
							
							if(!mav_mission_up_flag)
								joylink_msg_heartbeat_send_struct(JOYLINK_COMM_3, &heart_beat); //#0  /* 客户负载板连接串口2 */
								
							break;
						}
						/***  电池电量  通信信号强度 信息 #1***/
						case MAVLINK_MSG_ID_SYS_STATUS:   //#1  ok            
						{		
							joylink_power_status power_status;
							
							power_status.voltage_battery = mavlink_msg_sys_status_get_voltage_battery(&r_message);  //电压单位mv
							power_status.current_battery = mavlink_msg_sys_status_get_current_battery(&r_message);	 //*10   mA
							power_status.battery_remaining = mavlink_msg_sys_status_get_battery_remaining(&r_message);
							
//							if(!mav_mission_up_flag)
								joylink_msg_power_status_send_struct(JOYLINK_COMM_3, &power_status);
						
							break;
						}
						
						/***	系统运行时间	***/
						case MAVLINK_MSG_ID_SYSTEM_TIME :   //#2 
						{
							if(!mav_mission_up_flag)
							{
								joylink_system_time system_time;
								
								system_time.time_unix_usec = mavlink_msg_system_time_get_time_unix_usec(&r_message);  
								system_time.time_boot_ms = mavlink_msg_system_time_get_time_boot_ms(&r_message);	 
								
								joylink_msg_system_time_send_struct(JOYLINK_COMM_3, &system_time); 
							}
	
							break;
						}
						
						/*** Metrics typically displayed on a HUD for fixed wing aircraft ***/
						case  MAVLINK_MSG_ID_VFR_HUD:  //#74 
						{
							if(!mav_mission_up_flag)
							{
								joylink_fly_status fly_status;
								
								fly_status.airspeed = mavlink_msg_vfr_hud_get_airspeed(&r_message);
								fly_status.groundspeed = mavlink_msg_vfr_hud_get_groundspeed(&r_message);
								fly_status.alt = mavlink_msg_vfr_hud_get_alt(&r_message);
								fly_status.climb = mavlink_msg_vfr_hud_get_climb(&r_message);
								fly_status.heading = mavlink_msg_vfr_hud_get_heading(&r_message);
								fly_status.throttle = mavlink_msg_vfr_hud_get_throttle(&r_message);
								
								joylink_msg_fly_status_send_struct(JOYLINK_COMM_3, &fly_status); 
							}
														
							break;
						}
						
						/***  飞行器的位置信息 ***/
						case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:   //#33  ok
						{	
							if(!mav_mission_up_flag)
							{								
								joylink_gps_position gps_pos;

								gps_pos.alt = mavlink_msg_global_position_int_get_alt(&r_message);
								gps_pos.hdg = mavlink_msg_global_position_int_get_hdg(&r_message);
								gps_pos.lat = mavlink_msg_global_position_int_get_lat(&r_message);
								gps_pos.lon = mavlink_msg_global_position_int_get_lon(&r_message);
								gps_pos.relative_alt = mavlink_msg_global_position_int_get_relative_alt(&r_message);
								gps_pos.time_boot_ms = mavlink_msg_global_position_int_get_time_boot_ms(&r_message);
								gps_pos.vx = mavlink_msg_global_position_int_get_vx(&r_message);
								gps_pos.vy = mavlink_msg_global_position_int_get_vy(&r_message);
								gps_pos.vz = mavlink_msg_global_position_int_get_vz(&r_message);
								
								joylink_msg_gps_position_send_struct(JOYLINK_COMM_3, &gps_pos); 
							}
							
							break;
						}
						
						case MAVLINK_MSG_ID_GPS_RAW_INT:  //#24  ok
						{
							if(!mav_mission_up_flag)
							{
								joylink_gps_raw gps_raw;
								
								gps_raw.alt = mavlink_msg_gps_raw_int_get_alt(&r_message);
								gps_raw.cog = mavlink_msg_gps_raw_int_get_cog(&r_message);
								gps_raw.eph = mavlink_msg_gps_raw_int_get_eph(&r_message);
								gps_raw.epv = mavlink_msg_gps_raw_int_get_epv(&r_message);
								gps_raw.fix_type = mavlink_msg_gps_raw_int_get_fix_type(&r_message);
								gps_raw.lat = mavlink_msg_gps_raw_int_get_lat(&r_message);
								gps_raw.lon = mavlink_msg_gps_raw_int_get_lon(&r_message);
								gps_raw.time_usec = mavlink_msg_gps_raw_int_get_time_usec(&r_message);
								gps_raw.vel = mavlink_msg_gps_raw_int_get_vel(&r_message);
								gps_raw.satellites_visible = mavlink_msg_gps_raw_int_get_satellites_visible(&r_message);
								
								
								joylink_msg_gps_raw_send_struct(JOYLINK_COMM_3, &gps_raw); 
							}
							
							break;
						}
						
						case MAVLINK_MSG_ID_MISSION_CURRENT: //#42 ok
						{
							if(!mav_mission_up_flag)
							{
								joylink_mission_current mission;
								
								mission.seq = mavlink_msg_mission_current_get_seq(&r_message);

								joylink_msg_mission_current_send_struct(JOYLINK_COMM_3, &mission); 
							}
							
							break;
						}
						
						case MAVLINK_MSG_ID_COMMAND_ACK: //#77  ok
						{
							if(!mav_mission_up_flag)
							{
								joylink_command_ack command_ack;					
								
								command_ack.command = mavlink_msg_command_ack_get_command(&r_message);
								command_ack.result = mavlink_msg_command_ack_get_result(&r_message);
							
								joylink_msg_command_ack_send_struct(JOYLINK_COMM_3, &command_ack);
							}
							break;
						}
						case MAVLINK_MSG_ID_MISSION_CLEAR_ALL: //#45
						{
							if(!mav_mission_up_flag)
							{
								joylink_mission_clear_all mission_clear_all;
								
								mission_clear_all.target_system = mavlink_msg_mission_clear_all_get_target_system(&r_message);
								mission_clear_all.target_id = mavlink_msg_mission_clear_all_get_target_component(&r_message);
								
								joylink_msg_mission_clear_all_send_struct(JOYLINK_COMM_3, &mission_clear_all);
							}
							break;
						}
						
					  case MAVLINK_MSG_ID_HOME_POSITION: //#242
						{
							if(!mav_mission_up_flag)
							{
								joylink_home_position home_pos;
								
								home_pos.altitude = mavlink_msg_home_position_get_altitude(&r_message);
								home_pos.approach_x = mavlink_msg_home_position_get_x(&r_message);
								home_pos.approach_y = mavlink_msg_home_position_get_y(&r_message);
								home_pos.approach_z = mavlink_msg_home_position_get_z(&r_message);
								home_pos.latitude = mavlink_msg_home_position_get_latitude(&r_message);
								home_pos.longitude = mavlink_msg_home_position_get_longitude(&r_message);
								home_pos.x = mavlink_msg_home_position_get_approach_x(&r_message);
								home_pos.y = mavlink_msg_home_position_get_approach_y(&r_message);
								home_pos.z = mavlink_msg_home_position_get_approach_z(&r_message);
								
								mavlink_msg_home_position_get_q(&r_message, &home_pos.q[0]);
								
								
							 joylink_msg_home_position_send_struct(JOYLINK_COMM_3, &home_pos);
							}
							
							break;
						}
						
						case MAVLINK_MSG_ID_MISSION_COUNT://#44  OK
						{
							joylink_mission_count mission_count;
							
							mission_count.count = mavlink_msg_mission_count_get_count(&r_message);
							mission_count.target_id = mavlink_msg_mission_count_get_target_component(&r_message);
							mission_count.target_system = mavlink_msg_mission_count_get_target_system(&r_message);
							
							mav_mission_up_flag = 1;  /* 接下来禁止发送发送不是航点的信息  */
							mav_mission_up_start_time = xTaskGetTickCount();/* 记录开始时间，如果超过38秒，恢复传送其他信息 */
			
							joylink_msg_mission_count_send_struct(JOYLINK_COMM_3, &mission_count);
							//usartSendString(USART1, "MAV MISSION COUNT\r\n");
							
							break;
						}
						
						case MAVLINK_MSG_ID_MISSION_ITEM: //#39  ok
						{
							joylink_mission_element mission_element;
							
							mission_element.autocontinue = mavlink_msg_mission_item_get_autocontinue(&r_message);
							mission_element.command = mavlink_msg_mission_item_get_command(&r_message);
							mission_element.current = mavlink_msg_mission_item_get_current(&r_message);
							mission_element.frame = mavlink_msg_mission_item_get_frame(&r_message);
							mission_element.param1 = mavlink_msg_mission_item_get_param1(&r_message);
							mission_element.param2 = mavlink_msg_mission_item_get_param2(&r_message);
							mission_element.param3 = mavlink_msg_mission_item_get_param3(&r_message);
							mission_element.param4 = mavlink_msg_mission_item_get_param4(&r_message);
							mission_element.seq = mavlink_msg_mission_item_get_seq(&r_message);
							mission_element.target_id = mavlink_msg_mission_item_get_target_component(&r_message);
							mission_element.target_system = mavlink_msg_mission_item_get_target_system(&r_message);
							mission_element.x = mavlink_msg_mission_item_get_x(&r_message);
							mission_element.y = mavlink_msg_mission_item_get_y(&r_message);
							mission_element.z = mavlink_msg_mission_item_get_z(&r_message);
					
							joylink_msg_mission_element_send_struct(JOYLINK_COMM_3, &mission_element);
							//usartSendString(USART1, "MISSION item\r\n");
							
							break;
						}
						
						case MAVLINK_MSG_ID_MISSION_REQUEST: //#40 
						{
							joylink_mission_request mission_request;
							
							mission_request.seq = mavlink_msg_mission_request_get_seq(&r_message);
							mission_request.target_id = mavlink_msg_mission_request_get_target_component(&r_message);
							mission_request.target_system = mavlink_msg_mission_request_get_target_system(&r_message);
													
							joylink_msg_mission_request_send_struct(JOYLINK_COMM_3, &mission_request);				
							
							break;
						}
						
						case MAVLINK_MSG_ID_MISSION_REQUEST_LIST://#43
						{
							joylink_mission_request_list mission_request_list;
							
							mission_request_list.target_id = mavlink_msg_mission_request_list_get_target_component(&r_message);
							mission_request_list.target_system = mavlink_msg_mission_request_list_get_target_system(&r_message);
							
							joylink_msg_mission_request_list_send_struct(JOYLINK_COMM_3, &mission_request_list);								
							
							break;
						}
						
						case MAVLINK_MSG_ID_MISSION_ACK: //#47
						{
							joylink_mission_ack mission_ack;
							
							mission_ack.target_id = mavlink_msg_mission_ack_get_target_component(&r_message);
							mission_ack.target_system = mavlink_msg_mission_ack_get_target_system(&r_message);
							mission_ack.type = mavlink_msg_mission_ack_get_type(&r_message);
														
							joylink_msg_mission_ack_send_struct(JOYLINK_COMM_3, &mission_ack);
							
							if(mission_ack.type==MAV_MISSION_ACCEPTED)
							{
								mav_mission_up_flag = 0;
							}
							
							break;
						}
						
						case MAVLINK_MSG_ID_SCALED_PRESSURE: //#29
						{
							if(!mav_mission_up_flag)
							{
								joylink_pressure pressure;
								
								pressure.press_abs = mavlink_msg_scaled_pressure_get_press_abs(&r_message);
								pressure.press_diff = mavlink_msg_scaled_pressure_get_press_diff(&r_message);
								pressure.temperature = mavlink_msg_scaled_pressure_get_temperature(&r_message);
								pressure.time_boot_ms = mavlink_msg_scaled_pressure_get_time_boot_ms(&r_message);
								
								joylink_msg_pressure_send_struct(JOYLINK_COMM_3, &pressure);
							}
							
							break;
						}
						
						case MAVLINK_MSG_ID_ATTITUDE: //#30
						{
							if(!mav_mission_up_flag)
							{
								joylink_attitude attitude;
								
								attitude.pitch = mavlink_msg_attitude_get_pitch(&r_message);
								attitude.pitchspeed = mavlink_msg_attitude_get_pitchspeed(&r_message);
								attitude.roll = mavlink_msg_attitude_get_roll(&r_message); 
								attitude.rollspeed = mavlink_msg_attitude_get_rollspeed(&r_message);
								attitude.time_boot_ms = mavlink_msg_attitude_get_time_boot_ms(&r_message);
								attitude.yaw = mavlink_msg_attitude_get_yaw(&r_message);
								attitude.yawspeed = mavlink_msg_attitude_get_yawspeed(&r_message);
								
								joylink_msg_attitude_send_struct(JOYLINK_COMM_3, &attitude);
							}
							
							break;
						}
						
						case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT: //#62
						{
							if(!mav_mission_up_flag)
							{
								joylink_nav_controller_output controller_output;
								
								controller_output.alt_error = mavlink_msg_nav_controller_output_get_alt_error(&r_message);
								controller_output.aspd_error = mavlink_msg_nav_controller_output_get_aspd_error(&r_message);
								controller_output.nav_bearing = mavlink_msg_nav_controller_output_get_nav_bearing(&r_message);
								controller_output.nav_pitch = mavlink_msg_nav_controller_output_get_nav_pitch(&r_message);
								controller_output.nav_roll = mavlink_msg_nav_controller_output_get_nav_roll(&r_message);
								controller_output.target_bearing = mavlink_msg_nav_controller_output_get_target_bearing(&r_message);
								controller_output.wp_dist = mavlink_msg_nav_controller_output_get_wp_dist(&r_message);
								controller_output.xtrack_error = mavlink_msg_nav_controller_output_get_xtrack_error(&r_message);
								
								joylink_msg_nav_controller_output_send_struct(JOYLINK_COMM_3, &controller_output);
							}
						}
							
						case MAVLINK_MSG_ID_RANGEFINDER: //#173
						{
							if(!mav_mission_up_flag)
							{
								joylink_rangefinder rangefinder;
								
								rangefinder.distance = mavlink_msg_rangefinder_get_distance(&r_message);
								rangefinder.voltage = mavlink_msg_rangefinder_get_voltage(&r_message);
								
								joylink_msg_rangefinder_send_struct(JOYLINK_COMM_3, &rangefinder);
							}
							
							break;
						}
						
						case MAVLINK_MSG_ID_MISSION_ITEM_REACHED: //#46
						{
							if(!mav_mission_up_flag)
							{
								joylink_mission_item_reached reach;
								
								reach.seq = mavlink_msg_mission_item_reached_get_seq(&r_message);
								
								joylink_msg_mission_item_reached_send_struct(JOYLINK_COMM_3, &reach);
							}
							
							break;
						}
						
						case MAVLINK_MSG_ID_CAMERA_FEEDBACK: //#180
						{
							joylink_camera_feedback camera_feedback;
							
							camera_feedback.alt_msl = mavlink_msg_camera_feedback_get_alt_msl(&r_message);
							camera_feedback.alt_rel = mavlink_msg_camera_feedback_get_alt_rel(&r_message);
							camera_feedback.cam_idx = mavlink_msg_camera_feedback_get_cam_idx(&r_message);
							camera_feedback.flags = mavlink_msg_camera_feedback_get_flags(&r_message);
							camera_feedback.foc_len = mavlink_msg_camera_feedback_get_foc_len(&r_message);
							camera_feedback.img_idx = mavlink_msg_camera_feedback_get_img_idx(&r_message);
							camera_feedback.lat = mavlink_msg_camera_feedback_get_lat(&r_message);
							camera_feedback.lng = mavlink_msg_camera_feedback_get_lng(&r_message);
							camera_feedback.pitch = mavlink_msg_camera_feedback_get_pitch(&r_message);
							camera_feedback.roll = mavlink_msg_camera_feedback_get_roll(&r_message);
							camera_feedback.target_system = mavlink_msg_camera_feedback_get_target_system(&r_message);
							camera_feedback.time_usec = mavlink_msg_camera_feedback_get_time_usec(&r_message);
							camera_feedback.yaw = mavlink_msg_camera_feedback_get_yaw(&r_message);
							
							joylink_msg_camera_feedback_send_struct(JOYLINK_COMM_3, &camera_feedback);
							
							break;
						}						
						
						default: break;
												
					}
				}
			}
		}
}

void usart1_receive_task(void *pvParameters)
{
	mavlink_status_t r_mavlink_status;    //解析成功后的数据帧状态标志位
	mavlink_message_t r_message;					//解析成功的数据帧
	mavlink_framing_t parse_result = MAVLINK_FRAMING_INCOMPLETE;    //解析函数返回的结果

	u8 ch =0;
	
	while(1)
	{
		if(pdPASS==xQueueReceive(com1_rx_queue_handle, &ch, portMAX_DELAY))
		{
			/* 0，1，2分别代表串口通道1、2、3*/
			parse_result = (mavlink_framing_t)mavlink_parse_char(JOYLINK_COMM_1, ch, &r_message, &r_mavlink_status);  //解析一个字符
			
			if(MAVLINK_FRAMING_OK==parse_result)    //成功解析一帧数据
			{
				gcs_mavlink_system.sysid = r_message.sysid;
				gcs_mavlink_system.compid = r_message.compid+1;
				
				_mavlink_resend_uart(MAVLINK_COMM_3, &r_message);
			}
			
		}
	}
}

/*** USART3 串口数据处理任务 接控制板***/
void usart3_receive_task(void *pvParameters)
{
	joylink_status r_joylink_status;    //解析成功后的数据帧状态标志位
	joylink_message r_message;					//解析成功的数据帧
	joylink_framing parse_result = JOYLINK_FRAMING_INCOMPLETE;    //解析函数返回的结果
	
	mavlink_message_t mavlink_msg;      //待发送的mavlink信息
	
	char ch;
	portBASE_TYPE xStatus;
	
	#if AWESOME_DEBUG
		char str[50];
	#endif
		
	while(1)
	{	
		xStatus = xQueueReceive(com3_rx_queue_handle, &ch, 25/portTICK_RATE_MS );

		if(pdPASS==xStatus)
		{	
			/* 0，1，2分别代表串口通道1、2、3*/
			parse_result = (joylink_framing) joylink_frame_char(JOYLINK_COMM_3, ch, &r_message, &r_joylink_status);  //解析一个字符
			
			if(JOYLINK_FRAMING_OK==parse_result)    //成功解析一帧数据
			{
				
				switch(r_message.msgid)
				{
					
					case JOYLINK_MSG_ID_SET_MODE: //#11
					{
						mavlink_set_mode_t set_mode;
						
						set_mode.base_mode = joylink_msg_set_mode_get_base_mode(&r_message);
						set_mode.custom_mode = joylink_msg_set_mode_get_flight_mode(&r_message);
						set_mode.target_system = joylink_msg_set_mode_get_target_system(&r_message);
						
						mavlink_msg_set_mode_encode(board_mavlink_system.sysid, board_mavlink_system.compid, &mavlink_msg, &set_mode);
						
						break;
					}
					
					case JOYLINK_MSG_ID_COMMAND:  //joylink #76   ok
					{
						mavlink_command_long_t command_long;
																			
						command_long.command = joylink_msg_command_get_command(&r_message);
						command_long.confirmation = joylink_msg_command_get_confirmation(&r_message);
						command_long.param1 = joylink_msg_command_get_param1(&r_message);
						command_long.param2 = joylink_msg_command_get_param2(&r_message);
						command_long.param3 = joylink_msg_command_get_param3(&r_message);
						command_long.param4 = joylink_msg_command_get_param4(&r_message);
						command_long.param5 = joylink_msg_command_get_param5(&r_message);
						command_long.param6 = joylink_msg_command_get_param6(&r_message);
						command_long.param7 = joylink_msg_command_get_param7(&r_message);
						command_long.target_component = joylink_msg_command_get_target_id(&r_message);
						command_long.target_system = joylink_msg_command_get_target_system(&r_message);
						
						#if AWESOME_DEBUG
							usartSendString(USART1, "code: command \r\n");
						#endif
						
						mavlink_msg_command_long_encode(board_mavlink_system.sysid, board_mavlink_system.compid, &mavlink_msg, &command_long);
						
						break;
					}
					
					case JOYLINK_MSG_ID_MISSION_SET_CURRENT:  //#41  ok
					{
						mavlink_mission_set_current_t mission_set_current;
						
						mission_set_current.seq = joylink_msg_mission_set_current_get_seq(&r_message);
						mission_set_current.target_component = joylink_msg_mission_set_current_get_target_id(&r_message);
						mission_set_current.target_system = joylink_msg_mission_set_current_get_target_system(&r_message);
						
						#if AWESOME_DEBUG
							usartSendString(USART1, "code: set current mission\r\n");
						#endif
						
						mavlink_msg_mission_set_current_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &mission_set_current);
						
						break;
					}
					
					case JOYLINK_MSG_ID_MISSION_CLEAR_ALL: //#45  ok
					{
						mavlink_mission_clear_all_t mission_clear_all;
						
						mission_clear_all.target_component = joylink_msg_mission_clear_all_get_target_id(&r_message);
						mission_clear_all.target_system = joylink_msg_mission_clear_all_get_target_system(&r_message);
						
						#if AWESOME_DEBUG
							usartSendString(USART1, "code: clear all mission\r\n");
						#endif
						
						mavlink_msg_mission_clear_all_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &mission_clear_all);
						
						break;
					}
					
					case JOYLINK_MSG_ID_MISSION_COUNT:  //#44   ok
					{
						mavlink_mission_count_t mission_count;   					
						mission_count.count = joylink_msg_mission_count_get_count(&r_message);
						mission_count.target_component = joylink_msg_mission_count_get_target_id(&r_message);
						mission_count.target_system = joylink_msg_mission_count_target_system(&r_message);
						
						xQueueReset(com2_rx_queue_handle); /* 清空队列中的数据，加快传送航点数据 */
						mav_mission_up_flag = 1;  /* 接下来禁止发送发送不是航点的信息  */
						mav_mission_up_start_time = xTaskGetTickCount();/* 记录开始时间，如果超过38秒，恢复传送其*/
						
						mavlink_msg_mission_count_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &mission_count);
						
							
						#if AWESOME_DEBUG
							sprintf(str, "mission count = %d\r\n", mission_count.count);
							usartSendString(USART1, str);
						#endif
												
						break;
					}			

					case JOYLINK_MSG_ID_MISSION_ELEMENT:  //#39
					{
						mavlink_mission_item_t mission_item;						
						
						mission_item.autocontinue = joylink_msg_mission_element_get_autocontinue(&r_message);
						mission_item.command = joylink_msg_mission_element_get_command(&r_message);
						mission_item.current = joylink_msg_mission_element_get_current(&r_message);
						mission_item.frame = joylink_msg_mission_element_get_frame(&r_message);
						mission_item.param1 = joylink_msg_mission_element_get_param1(&r_message);
						mission_item.param2 = joylink_msg_mission_element_get_param2(&r_message);
						mission_item.param3 = joylink_msg_mission_element_get_param3(&r_message);
						mission_item.param4 = joylink_msg_mission_element_get_param4(&r_message);
						mission_item.seq = joylink_msg_mission_element_get_seq(&r_message);
						mission_item.target_component = joylink_msg_mission_element_get_target_id(&r_message);
						mission_item.target_system =  joylink_msg_mission_element_get_target_system(&r_message);
						mission_item.x = joylink_msg_mission_element_get_x(&r_message);
						mission_item.y = joylink_msg_mission_element_get_y(&r_message);
						mission_item.z = joylink_msg_mission_element_get_z(&r_message);
						
						#if AWESOME_DEBUG
							sprintf(str, "send the mission item seq= %d\r\n", mission_item.seq);
							usartSendString(USART1, str);
						#endif
												
						mavlink_msg_mission_item_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &mission_item);
					
						break;
					}
					
					case JOYLINK_MSG_ID_MISSION_REQUEST:  //#40
					{
						mavlink_mission_request_t mission_request;
						
						mission_request.seq = joylink_msg_mission_request_get_seq(&r_message);
						mission_request.target_component = joylink_msg_mission_request_get_target_id(&r_message);
						mission_request.target_system = joylink_msg_mission_request_get_target_system(&r_message);	
						
						#if AWESOME_DEBUG
							usartSendString(USART1, "code: mission request seq\r\n");
						#endif

						mavlink_msg_mission_request_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &mission_request);						
						
						break;
					}
					
					case JOYLINK_MSG_ID_MISSION_REQUEST_LIST:  //#43  OK
					{
						mavlink_mission_request_list_t mission_request_list;
						mission_request_list.target_component = joylink_msg_mission_request_list_get_target_id(&r_message);	
						mission_request_list.target_system = joylink_msg_mission_request_list_get_target_system(&r_message);	
						
						#if AWESOME_DEBUG
							usartSendString(USART1, "code: request mission list\r\n");
						#endif
										
						mavlink_msg_mission_request_list_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &mission_request_list);
						
						break;
					}
					
					case JOYLINK_MSG_ID_MISSION_ACK:  //#47  OK
					{
						mavlink_mission_ack_t mission_ack;
						
						mission_ack.target_component = joylink_msg_mission_ack_get_target_id(&r_message);
						mission_ack.target_system = joylink_msg_mission_ack_get_target_system(&r_message);
						mission_ack.type = joylink_msg_mission_ack_get_type(&r_message);
						
						#if AWESOME_DEBUG
							usartSendString(USART1, "code: mission ack\r\n");
						#endif
						
//						if(mission_ack.type==JOY_MISSION_ACCEPTED)
//						{
//							mav_mission_up_flag = 0;
//						}
						
						mavlink_msg_mission_ack_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &mission_ack);
						
						break;
					}
					
					case JOYLINK_MSG_ID_REQUEST_SEND_DATA: //#66
					{
						mavlink_request_data_stream_t request_data;
						
						request_data.req_message_rate = joylink_msg_request_send_data_get_req_message_rate(&r_message);
						request_data.req_stream_id = joylink_msg_request_send_data_get_req_stream_id(&r_message);
						request_data.start_stop = joylink_msg_request_send_data_get_start_stop(&r_message);
						request_data.target_component = joylink_msg_request_send_data_get_target_id(&r_message);
						request_data.target_system = joylink_msg_request_send_data_get_target_system(&r_message);
						
						mavlink_msg_request_data_stream_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &request_data);
						
						#if AWESOME_DEBUG
							usartSendString(USART1, "request send data stream\r\n");
						#endif
						
						break;
					}
					
					case JOYLINK_MSG_ID_SET_POSITION_TARGET_LOCAL_NED: //#84
					{
						mavlink_set_position_target_local_ned_t target_local_ned;
						
						target_local_ned.afx = joylink_msg_set_position_target_local_ned_get_afx(&r_message);
						target_local_ned.afy = joylink_msg_set_position_target_local_ned_get_afy(&r_message);
						target_local_ned.afz = joylink_msg_set_position_target_local_ned_get_afz(&r_message);
						target_local_ned.coordinate_frame = joylink_msg_set_position_target_local_ned_get_coordinate_frame(&r_message);
						target_local_ned.target_component = joylink_msg_set_position_target_local_ned_get_target_id(&r_message);
						target_local_ned.target_system = joylink_msg_set_position_target_local_ned_get_target_system(&r_message);
						target_local_ned.time_boot_ms = joylink_msg_set_position_target_local_ned_get_time_boot_ms(&r_message);
						target_local_ned.type_mask = joylink_msg_set_position_target_local_ned_get_type_mask(&r_message);
						target_local_ned.vx = joylink_msg_set_position_target_local_ned_get_vx(&r_message);
						target_local_ned.vy = joylink_msg_set_position_target_local_ned_get_vy(&r_message);
						target_local_ned.vz = joylink_msg_set_position_target_local_ned_get_vz(&r_message);
						target_local_ned.x = joylink_msg_set_position_target_local_ned_get_x(&r_message);
						target_local_ned.y = joylink_msg_set_position_target_local_ned_get_y(&r_message);
						target_local_ned.yaw = joylink_msg_set_position_target_local_ned_get_yaw(&r_message);
						target_local_ned.yaw_rate = joylink_msg_set_position_target_local_ned_get_yaw_rate(&r_message);
						target_local_ned.z = joylink_msg_set_position_target_local_ned_get_z(&r_message);
						
						mavlink_msg_set_position_target_local_ned_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &target_local_ned);
						
						break;
					}
					
					case JOYLINK_MSG_ID_SET_POSITION_TARGET_GLOBAL_INT: //#86
					{
						mavlink_set_position_target_global_int_t  position_target_global_int;
						
						position_target_global_int.afx = joylink_msg_set_position_target_global_int_get_afx(&r_message);
						position_target_global_int.afy = joylink_msg_set_position_target_global_int_get_afy(&r_message);
						position_target_global_int.afz = joylink_msg_set_position_target_global_int_get_afz(&r_message);
						position_target_global_int.alt = joylink_msg_set_position_target_global_int_get_alt(&r_message);
						position_target_global_int.coordinate_frame = joylink_msg_set_position_target_global_int_get_coordinate_frame(&r_message);
						position_target_global_int.lat_int = joylink_msg_set_position_target_global_int_get_lat_int(&r_message);
						position_target_global_int.lon_int = joylink_msg_set_position_target_global_int_get_lon_int(&r_message);
						position_target_global_int.target_component = joylink_msg_set_position_target_global_int_get_target_id(&r_message);
						position_target_global_int.target_system = joylink_msg_set_position_target_global_int_get_target_system(&r_message);
						position_target_global_int.time_boot_ms = joylink_msg_set_position_target_global_int_get_time_boot_ms(&r_message);
						position_target_global_int.type_mask = joylink_msg_set_position_target_global_int_get_type_mask(&r_message);
						position_target_global_int.vx = joylink_msg_set_position_target_global_int_get_vx(&r_message);
						position_target_global_int.vy = joylink_msg_set_position_target_global_int_get_vy(&r_message);
						position_target_global_int.vz = joylink_msg_set_position_target_global_int_get_vz(&r_message);
						position_target_global_int.yaw = joylink_msg_set_position_target_global_int_get_yaw(&r_message);
						position_target_global_int.yaw_rate = joylink_msg_set_position_target_global_int_get_yaw_rate(&r_message);
						
						mavlink_msg_set_position_target_global_int_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &position_target_global_int);					
						
						break;
					}
					
					case JOYLINK_MSG_ID_LANDING_TARGET: //#149
					{
						
						mavlink_landing_target_t landing_target;
						
						landing_target.angle_x = joylink_msg_landing_target_get_angle_x(&r_message);
						landing_target.angle_y = joylink_msg_landing_target_get_angle_y(&r_message);
						landing_target.distance = joylink_msg_landing_target_get_distance(&r_message);
						landing_target.frame = joylink_msg_landing_target_get_frame(&r_message);
						landing_target.size_x = joylink_msg_landing_target_get_size_x(&r_message);
						landing_target.size_y = joylink_msg_landing_target_get_size_y(&r_message);
						landing_target.target_num = joylink_msg_landing_target_get_target_num(&r_message);
						landing_target.time_usec = joylink_msg_landing_target_get_time_usec(&r_message);
					
						mavlink_msg_landing_target_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &landing_target);
						break;
					}
					
					case JOYLINK_MSG_ID_SET_HOME_POSITION: //#243
					{
						mavlink_set_home_position_t home_position ;
						
						home_position.altitude = joylink_msg_set_home_position_get_altitude(&r_message);
						home_position.approach_x = joylink_msg_set_home_position_get_x(&r_message);
						home_position.approach_y = joylink_msg_set_home_position_get_y(&r_message);
						home_position.approach_z = joylink_msg_set_home_position_get_z(&r_message);
						home_position.latitude = joylink_msg_set_home_position_get_latitude(&r_message);
						home_position.longitude = joylink_msg_set_home_position_get_longitude(&r_message);
						joylink_msg_set_home_position_get_q(&r_message, &home_position.q[0]);
						home_position.target_system = joylink_msg_set_home_position_get_target_system(&r_message);
						home_position.x = joylink_msg_set_home_position_get_approach_x(&r_message);
						home_position.y = joylink_msg_set_home_position_get_approach_y(&r_message);
						home_position.z = joylink_msg_set_home_position_get_approach_z(&r_message);
						
						mavlink_msg_set_home_position_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &home_position);
						
						break;
					}
					
					case JOYLINK_MSG_ID_RC_CHANNELS_OVERRIDE: //#70
					{
						mavlink_rc_channels_override_t rc;
						
						rc.chan1_raw = joylink_msg_rc_channels_override_get_chan1_raw(&r_message);
						rc.chan2_raw = joylink_msg_rc_channels_override_get_chan2_raw(&r_message);
						rc.chan3_raw = joylink_msg_rc_channels_override_get_chan3_raw(&r_message);
						rc.chan4_raw = joylink_msg_rc_channels_override_get_chan4_raw(&r_message);
						rc.chan5_raw = joylink_msg_rc_channels_override_get_chan5_raw(&r_message);
						rc.chan6_raw = joylink_msg_rc_channels_override_get_chan6_raw(&r_message);
						rc.chan7_raw = joylink_msg_rc_channels_override_get_chan7_raw(&r_message);
						rc.chan8_raw = joylink_msg_rc_channels_override_get_chan8_raw(&r_message);
						rc.target_component= joylink_msg_rc_channels_override_get_target_id(&r_message);
						rc.target_system = joylink_msg_rc_channels_override_get_target_system(&r_message);
						
						mavlink_msg_rc_channels_override_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &rc);					
						
						break;
					}
					
					case JOYLINK_MSG_ID_SET_ATTITUDE_TARGET:
					{
						mavlink_set_attitude_target_t attitude;
						
						attitude.body_pitch_rate = joylink_msg_set_attitude_target_get_body_pitch_rate(&r_message);
						attitude.body_roll_rate = joylink_msg_set_attitude_target_get_body_roll_rate(&r_message);
						attitude.body_yaw_rate = joylink_msg_set_attitude_target_get_body_yaw_rate(&r_message);
						joylink_msg_set_attitude_target_get_q(&r_message, &attitude.q[0]);
						attitude.target_component = joylink_msg_set_attitude_target_get_target_id(&r_message);
						attitude.target_system = joylink_msg_set_attitude_target_get_target_system(&r_message);
						attitude.thrust = joylink_msg_set_attitude_target_get_thrust(&r_message);
						attitude.time_boot_ms = joylink_msg_set_attitude_target_get_time_boot_ms(&r_message);
						attitude.type_mask = joylink_msg_set_attitude_target_get_type_mask(&r_message);
						
						mavlink_msg_set_attitude_target_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &attitude);	

						break;
					}
					
					case JOYLINK_MSG_ID_CAMERA_TRIGGER: //#112
					{
						mavlink_camera_trigger_t camera;
						
						camera.seq = joylink_msg_camera_trigger_get_seq(&r_message);
						camera.time_usec = joylink_msg_camera_trigger_get_time_usec(&r_message);
						
						mavlink_msg_camera_trigger_encode(board_mavlink_system.sysid , board_mavlink_system.compid, &mavlink_msg, &camera);
						break;
					}
					default: break;

					
				}
				_mavlink_resend_uart( MAVLINK_COMM_2 ,&mavlink_msg );
				
			}
		}
	}
}

void request_send_data(void)
{
	mavlink_request_data_stream_t r_send_data;
	
	r_send_data.req_message_rate = 2;
	r_send_data.req_stream_id = 2;
	r_send_data.start_stop = 1;
	r_send_data.target_component = 0x01;
	r_send_data.target_system = 0x01;
	
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	
	r_send_data.req_stream_id = 6; 
	r_send_data.req_message_rate = 2;	
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	
	r_send_data.req_stream_id = 10; 
	r_send_data.req_message_rate = 4;	
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	
	r_send_data.req_stream_id = 11; 
	r_send_data.req_message_rate = 4;	
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	
		r_send_data.req_stream_id = 12; 
	r_send_data.req_message_rate = 2;	
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	
		r_send_data.req_stream_id = 1; 
	r_send_data.req_message_rate = 2;	
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	
	r_send_data.req_stream_id = 3; 
	r_send_data.req_message_rate = 2;	
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	mavlink_msg_request_data_stream_send_struct(MAVLINK_COMM_2, &r_send_data);
	
}

