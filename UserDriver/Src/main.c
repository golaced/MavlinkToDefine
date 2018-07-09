
#include "includes.h"

#define SWOFT_VERSION "JOYLINK_TO_MAVLINKv1.4"

int main()
{
	taskDISABLE_INTERRUPTS();
	taskENTER_CRITICAL();	

	BSPInit();
	
	AppStart();	
	
	return 0;
}


