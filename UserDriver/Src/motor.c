
#include "motor.h"

/*---------------------------------------------------------------------
 * 配置LED用到的I/O口
 */
void Motor_IO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;  	

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   

	/* GPIOB8-9 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;         
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;          
	GPIO_Init(GPIOB, &GPIO_InitStructure);     

	GPIO_ResetBits(GPIOB,GPIO_Pin_6);     //输出化为低电平   芯片输出为高阻状态
	GPIO_ResetBits(GPIOB,GPIO_Pin_7); 
	GPIO_ResetBits(GPIOB,GPIO_Pin_8); 
	GPIO_ResetBits(GPIOB,GPIO_Pin_9); 
}

/*---------------------------------------------------------------------
 * 收放起落架控制，经过多次调试取得经验值上升时间为7.1s刚好不会使电机卡住
 * 下降时间为6.6s刚好不会使电机卡住
 */
	void Undercarriage_MotorControl(volatile u16 *ticks, u16 n)
	//void Undercarriage_MotorControl(void)
{
    static u8 s = 0;

    //if(c++ > 0)	//第一次进入函数无效
		//{
			if(s++%2)	//奇数次进入函数则控制起落架下降
			{  
				RIGHT_LEG_UP;
				LEFT_LEG_UP;
//				LED_OFF;
				ticks[2] = 8500;	//7.1s 下降时间
				//delay(t_1s);
			}
			else     //偶数次进入函数则控制起落架上升
			{				
				RIGHT_LEG_DOWN;
				LEFT_LEG_DOWN;
				ticks[1] = 8500;	//6.6s	//下架时间要快于上升时间
//				LED_ON;
			//	delay(t_1s);
			} 
			//c = 1;
   // }
        
    /* 起落架执行收放时不响应新指令*/
    while(ticks[1]||ticks[2])
		{  
			/*led on*/
			GPIO_ResetBits(GPIOB,GPIO_Pin_0);  
    }
    
    RIGHT_LEG_STOP;
    LEFT_LEG_STOP;
}


///*---------------------------------------------------------------------
// * 收放起落架控制
// */
//void Undercarriage_MotorStart(volatile u16 *ticks, u16 n, u8 direction)
//{  
//    if(direction == LEG_UP){
//        //RIGHT_LEG_UP;
//        //LEFT_LEG_UP;
//    }else if(direction == LEG_DOWN){
//        //RIGHT_LEG_DOWN;
//        //LEFT_LEG_DOWN;     
//    } 
//    /*led */
//    GPIO_ResetBits(GPIOB,GPIO_Pin_0);  
//    
//    ticks[1] = n;
//}


///*---------------------------------------------------------------------
// * 收放起落架控制
// */
//void Undercarriage_MotorStop(volatile u16 *ticks)
//{   
//    if(!ticks[1]){
//        RIGHT_LEG_STOP;
//        LEFT_LEG_STOP;
//        GPIO_SetBits(GPIOB,GPIO_Pin_0);
//    }  
//}



/*********************************************************************************/
