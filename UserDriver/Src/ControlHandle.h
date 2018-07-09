
#ifndef CONTROL_HANDLE_H
#define CONTROL_HANDLE_H

/*** 被控制对象的最大值，最小值（单向） ***/
#define OBJ_MAX	0x32       
#define OBJ_MIN 0

typedef struct _control_handle
{
	int controlMid;    //遥感中间位置量
	int controlMax;		//最大量程
	int controlMin;    //最小量程
	int currentVal;    //当前值
}controlHandle;

typedef enum _positon 
{
	LEFT = 0,
	RIGHT = 1	
}handlePosition;

static controlHandle controlHandles[2];   //左  右

	/**************************************
	*   设置遥感的初始参数
	*
	***************************************/
	void setControlHandleValue(controlHandle *phandle, int min, int mid, int max)
	{
		if(0==phandle||min<0||mid<=0||max<=0) return;
		phandle->controlMin = min;
		phandle->controlMid = mid;
		phandle->controlMax = max;
	}
	
	/**************************************
	*   水平位置遥感
	*
	***************************************/
	controlHandle* getControlHandle(unsigned char position)
	{
		return &controlHandles[position];
	}
	
	/**************************************
	*   返回当前值对应的控制量,
	*   通过返回正负值来判断方向
	***************************************/
	int getControlHandleVal(controlHandle *phandle)
	{
		float realVal = 0.0;
		
    realVal = (float)(phandle->currentVal-(phandle->controlMid));
		realVal = realVal*( (OBJ_MAX-OBJ_MIN)*1.0 / (phandle->controlMax-phandle->controlMid) );
		return (int)(realVal+0.5);	   //四舍五入	round函数
	}

#endif
