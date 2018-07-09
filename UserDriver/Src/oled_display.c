
#include "oled_display.h"
#include "NMEA0183.h"

/* 显示标题 */
void display_title(void)
{
	OLED_ShowString(TITLE_X,TITLE_Y,TITLE,16);
}

/* 显示系统运行时间 */
void display_runtime(void)
{
	unsigned char buff[RUNTIME_FONT_NUM+1];   //include the string end '\0'
	unsigned char offset = 0;
	
	offset = get_runtime_string((char*)buff,RUNTIME_FONT_NUM+1);
	
	OLED_ShowString(RUNTIME_X+offset*FONT_X_8,RUNTIME_Y,buff,16);
}

/* 显示GPS定位状态 */
void display_gps_state(void)
{
	unsigned char buff[GPS_STATE_FONT_NUM+1];   //include the string end '\0'
	gps_message *gpsMsg;
	
	copy_string(buff, GPS_STATE_TITLE, sizeof(GPS_STATE_TITLE)/sizeof(char)-1);
	
	gpsMsg = get_gps_message();
	if(true==gpsMsg->data_val)
	{
		copy_string(buff+sizeof(GPS_STATE_TITLE)/sizeof(char)-1, "YES ", sizeof("YES ")/sizeof(char)-1);
	}else
	{
		copy_string(buff+sizeof(GPS_STATE_TITLE)/sizeof(char)-1, "NONE", sizeof("NONE")/sizeof(char)-1);
	}
	buff[GPS_STATE_FONT_NUM-1] = '\0';
	
	OLED_ShowString(GPS_STATE_X,GPS_STATE_Y,buff,16);
}

/* 显示GPS定位类型 */
void display_gps_fix_class(void)
{
	unsigned char buff[GPS_FIX_CLASS_FONT_NUM+1];   //include the string end '\0'
	gps_message *gpsMsg;
	
	copy_string(buff, GPS_FIX_CLASS_TITLE, sizeof(GPS_FIX_CLASS_TITLE)/sizeof(char)-1);
	
	gpsMsg = get_gps_message();
	if('2'==gpsMsg->fix_mode)
	{
		copy_string(buff+sizeof(GPS_FIX_CLASS_TITLE)/sizeof(char)-1, "2D  ", sizeof("2D  ")/sizeof(char)-1);
	}else if('3'==gpsMsg->fix_mode)
	{
		copy_string(buff+sizeof(GPS_FIX_CLASS_TITLE)/sizeof(char)-1, "3D  ", sizeof("3D  ")/sizeof(char)-1);
	}else
	{
		copy_string(buff+sizeof(GPS_FIX_CLASS_TITLE)/sizeof(char)-1, "NONE", sizeof("NONE")/sizeof(char)-1);
	}
	buff[GPS_FIX_CLASS_FONT_NUM-1] = '\0';
	
	OLED_ShowString(GPS_FIX_CLASS_X,GPS_FIX_CLASS_Y,buff,16);
}  

/* 显示GPS卫星数量信息 */
void display_gps_count(void)
{
	unsigned char buff[GPS_STLLT_FONT_NUM+1];   //include the string end '\0'
	gps_message *gpsMsg;
	
	copy_string(buff, GPS_STLLT_TITLE, sizeof(GPS_STLLT_TITLE)/sizeof(char)-1);
	
	gpsMsg = get_gps_message();
	
	buff[sizeof(GPS_STLLT_TITLE)/sizeof(char)-1] = CHAR_BY_NUM((gpsMsg->scan_satellite_count%100)/10);
	buff[sizeof(GPS_STLLT_TITLE)/sizeof(char)] = CHAR_BY_NUM((gpsMsg->scan_satellite_count%100)%10);
	buff[sizeof(GPS_STLLT_TITLE)/sizeof(char)+1] = '/';
	buff[sizeof(GPS_STLLT_TITLE)/sizeof(char)+2] = CHAR_BY_NUM((gpsMsg->use_satellite_count%100)/10);
	buff[sizeof(GPS_STLLT_TITLE)/sizeof(char)+3] = CHAR_BY_NUM((gpsMsg->use_satellite_count%100)%10);
	buff[sizeof(GPS_STLLT_TITLE)/sizeof(char)+4] = '\0';
	
	OLED_ShowString(GPS_STLLT_X,GPS_STLLT_Y,buff,16);
}	

/* 显示纬度 */
void display_gps_lat(void)
{
	unsigned char buff[GPS_LAT_FONT_NUM+1];   //include the string end '\0'
	gps_message *gpsMsg;
	
	gpsMsg = get_gps_message();
	
	copy_string(buff, GPS_LAT_TITLE, sizeof(GPS_LAT_TITLE)/sizeof(char)-1);
	
	copy_string(buff+sizeof(GPS_LAT_TITLE)/sizeof(char)-1, get_gps_latitude_str(), 10);
	
	buff[sizeof(GPS_LAT_TITLE)/sizeof(char)-1+10] = (char)gpsMsg->lat_direc;
	buff[sizeof(GPS_LAT_TITLE)/sizeof(char)-1+11] = '\0';
	
	OLED_ShowString(GPS_LAT_X,GPS_LAT_Y,buff,16);
}
//end function

/*显示经度*/
void display_gps_lon(void)
{
	unsigned char buff[GPS_LON_FONT_NUM+1];   //include the string end '\0'
	gps_message *gpsMsg;
	
	gpsMsg = get_gps_message();
	
	copy_string(buff, GPS_LON_TITLE, sizeof(GPS_LON_TITLE)/sizeof(char)-1);
	
	copy_string(buff+sizeof(GPS_LON_TITLE)/sizeof(char)-1, get_gps_longitude_str(), 11);
	
	buff[sizeof(GPS_LON_TITLE)/sizeof(char)-1+11] = (char)gpsMsg->long_direc;
	buff[sizeof(GPS_LON_TITLE)/sizeof(char)-1+12] = '\0';
	
	OLED_ShowString(GPS_LON_X,GPS_LON_Y,buff,16);
}

/* 显示高度 */
void display_gps_alt(void)
{
	unsigned char buff[GPS_ALT_FONT_NUM+1];   //include the string end '\0'
	gps_message *gpsMsg;
	unsigned char len = 0;
	
	gpsMsg = get_gps_message();
	len = *(get_gps_altitude_str());
	
	copy_string(buff, GPS_ALT_TITLE, sizeof(GPS_ALT_TITLE)/sizeof(char)-1);
	
	copy_string(buff+sizeof(GPS_ALT_TITLE)/sizeof(char)-1, get_gps_altitude_str()+1, len);
	
	buff[sizeof(GPS_ALT_TITLE)/sizeof(char)-1+len] = 'M';
	buff[sizeof(GPS_ALT_TITLE)/sizeof(char)-1+len+1] = '\0';
	
	OLED_ShowString(GPS_ALT_X,GPS_ALT_Y,buff,16);
}
//end function

void display_gps_pdop(void)
{
	unsigned char buff[GPS_PDOP_FONT_NUM+1];   //include the string end '\0'
	gps_message *gpsMsg;
	
	gpsMsg = get_gps_message();
	
	copy_string(buff, GPS_PDOP_TITLE, sizeof(GPS_PDOP_TITLE)/sizeof(char)-1);
	
	copy_string(buff+sizeof(GPS_PDOP_TITLE)/sizeof(char)-1, get_gps_pdop_str(), 4);
	
	buff[sizeof(GPS_PDOP_TITLE)/sizeof(char)-1+4] = '\0';
	
	OLED_ShowString(GPS_PDOP_X,GPS_PDOP_Y,buff,16);
}
void display_gps_hdop(void)
{
	unsigned char buff[GPS_HDOP_FONT_NUM+1];   //include the string end '\0'
	gps_message *gpsMsg;
	
	gpsMsg = get_gps_message();
	
	copy_string(buff, GPS_HDOP_TITLE, sizeof(GPS_HDOP_TITLE)/sizeof(char)-1);
	
	copy_string(buff+sizeof(GPS_HDOP_TITLE)/sizeof(char)-1, get_gps_hdop_str(), 4);
	
	buff[sizeof(GPS_HDOP_TITLE)/sizeof(char)-1+4] = '\0';
	
	OLED_ShowString(GPS_HDOP_X,GPS_HDOP_Y,buff,16);
}
void display_gps_vdop(void)
{
	unsigned char buff[GPS_VDOP_FONT_NUM+1];   //include the string end '\0'
	gps_message *gpsMsg;
	
	gpsMsg = get_gps_message();
	
	copy_string(buff, GPS_VDOP_TITLE, sizeof(GPS_VDOP_TITLE)/sizeof(char)-1);
	
	copy_string(buff+sizeof(GPS_VDOP_TITLE)/sizeof(char)-1, get_gps_vdop_str(), 4);
	
	buff[sizeof(GPS_VDOP_TITLE)/sizeof(char)-1+4] = '\0';
	
	OLED_ShowString(GPS_VDOP_X,GPS_VDOP_Y,buff,16);
}

void display_stll_class(gps_message* msg)
{
	if(0!=msg)
	{
		OLED_DrawRadioBmp(0, 3, (msg->sttl_class&0x01));
		OLED_ShowChar(22,3,'G',16);
		OLED_ShowChar(30,3,'P',16);
		
		OLED_DrawRadioBmp(44, 3, ((msg->sttl_class&0x02)>>1));
		OLED_ShowChar(66,3,'G',16);
		OLED_ShowChar(74,3,'L',16);
		
		OLED_DrawRadioBmp(88, 3, (msg->sttl_class&0x04)>>2);
		OLED_ShowChar(110,3,'G',16);
		OLED_ShowChar(118,3,'B',16);
	}
}

static void copy_string(char* dest,char *src,unsigned int num) 
{ 
	if(dest!=0&&src!=0)
	{ while(num--&&'\0'!=*src)
		{
			*dest = *src;
			++dest;
			++src;
		} 
	}
} 

