#include "motor.h"
#include "sys.h"
#include "timer.h"
#include "pid.h"



void Motor_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PC端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;	//LED1-->PE.5 端口配置, 推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
}

void Motor_dir(u8 dir)//电机正反转控制函数
{
	if(dir)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_2);
		GPIO_ResetBits(GPIOC,GPIO_Pin_3);
	}else{
		GPIO_SetBits(GPIOC,GPIO_Pin_3);
		GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	}
}

void Motor_Set_Speed(float speed)//设定速度函数
{
	
}

float Motor_Read_Speed(void)//获取当前速度函数
{
	
}

void Motor_Set_Angle(u16 Angle)//设定转过角度
{
	
}

u16 Motor_Read_Angle(void)//获取当前已经转过角度
{
	
}

