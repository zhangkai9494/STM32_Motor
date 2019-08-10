#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "sys.h"

void Motor_Init(void);
u8 Motor_dir(u8 dir);//电机正反转控制函数
void Motor_Set_Speed(float speed);//设定速度函数
float Motor_Read_Speed(void);//获取当前速度函数
void Motor_Set_Angle(u16 Angle);//设定转过角度
float Motor_Read_Angle(void);//获取当前已经转过角度

#endif
