#ifndef __MOTOR_H_
#define __MOTOR_H_

#include "sys.h"

void Motor_Init(void);
void Motor_dir(u8 dir);//�������ת���ƺ���
void Motor_Set_Speed(float speed);//�趨�ٶȺ���
float Motor_Read_Speed(void);//��ȡ��ǰ�ٶȺ���
void Motor_Set_Angle(u16 Angle);//�趨ת���Ƕ�
u16 Motor_Read_Angle(void);//��ȡ��ǰ�Ѿ�ת���Ƕ�

#endif
