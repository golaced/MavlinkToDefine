
#ifndef RUNTIME_H
#define RUNTIME_H

	#include <stdbool.h>

	#define TIMER_TIME 10    //定时器时间ms，如果精确到一秒，则该值必须小于1000ms
	#define NUM_CHAR(x) "0123456789"[x]

	typedef struct _runtime
	{
		unsigned char hour;
		unsigned char minute;
		unsigned char second;
	}runtime;
		
  void time_count_increment(void);  //计数值自增
	
	bool get_runtime_by_count(runtime* time, unsigned int count);  //根据计数器计数值获取时间
	bool get_runtime(runtime* time);
	
	unsigned char format_time(runtime* time, char* buff, unsigned char len);  //格式化时间 hh:mm:ss 或者 mm:ss，之后的字符串存储到buff中，'\0'结尾
	static bool double_num_convert_str(unsigned char src, char* buff); //将二位数转换成字符保存在buff中
	
	unsigned char get_runtime_string(char* buff,unsigned int len);    //获取内部运行时间字符串
	
	extern unsigned int time_count;
	extern runtime time;

#endif
