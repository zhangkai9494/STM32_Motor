#include "motor.h"
#include "sys.h"
#include "timer.h"
#include "pid.h"
#include "usart.h"

void Motor_Init(void)//�����ʼ������|��ʼ��IO��
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��PC�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;//PC2.3�˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);//������� ��IO���ٶ�Ϊ50MHz
}


u8 Motor_dir(u8 dir)//�������ת���ƺ���|Ӳ������L298N����ʹ��ʵ�ֿ���
{
	if(dir)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_2);
		GPIO_ResetBits(GPIOC,GPIO_Pin_3);
	}else{
		GPIO_SetBits(GPIOC,GPIO_Pin_3);
		GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	}
	return dir;//����ת���־λ
}


void Motor_Set_Speed(float speed)//�趨�ٶȺ���|���ú���PID����һ��
{
	extern int skap;
	skap += PID_realize(speed,Motor_Read_Speed());
}


float Motor_Read_Speed(void)//��ȡ��ǰ�ٶȺ���|�����,ʵ�ַ�����timer.c
{
	extern float Speed;
	return Speed;
}


void Motor_Set_Angle(u16 Angle)//�趨ת���Ƕ�
{
	extern int skap;
	Motor_Set_Speed(25);
}


float Motor_Read_Angle(void)//��ȡ��ǰ�Ѿ�ת���Ƕ�
{
	extern u16 angle;
	return angle*1.07;
}
