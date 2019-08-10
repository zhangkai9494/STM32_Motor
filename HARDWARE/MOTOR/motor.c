#include "motor.h"
#include "sys.h"
#include "timer.h"
#include "pid.h"
#include "usart.h"

void Motor_Init(void)//电机初始化函数|初始化IO口
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能PC端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;//PC2.3端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//推挽输出 ，IO口速度为50MHz
}


u8 Motor_dir(u8 dir)//电机正反转控制函数|硬件上与L298N搭配使用实现控制
{
	if(dir)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_2);
		GPIO_ResetBits(GPIOC,GPIO_Pin_3);
	}else{
		GPIO_SetBits(GPIOC,GPIO_Pin_3);
		GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	}
	return dir;//返回转向标志位
}


void Motor_Set_Speed(float speed)//设定速度函数|调用后用PID调整一次
{
	extern int skap;
	skap += PID_realize(speed,Motor_Read_Speed());
}


float Motor_Read_Speed(void)//获取当前速度函数|输出用,实现方法在timer.c
{
	extern float Speed;
	return Speed;
}


void Motor_Set_Angle(u16 Angle)//设定转过角度
{
	extern int skap;
	Motor_Set_Speed(25);
}


float Motor_Read_Angle(void)//获取当前已经转过角度
{
	extern u16 angle;
	return angle*1.07;
}
