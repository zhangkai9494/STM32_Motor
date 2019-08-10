#include "motor.h"
#include "sys.h"
#include "timer.h"
#include "pid.h"



void Motor_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��PC�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;	//LED1-->PE.5 �˿�����, �������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	  				 //������� ��IO���ٶ�Ϊ50MHz
}

void Motor_dir(u8 dir)//�������ת���ƺ���
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

void Motor_Set_Speed(float speed)//�趨�ٶȺ���
{
	
}

float Motor_Read_Speed(void)//��ȡ��ǰ�ٶȺ���
{
	
}

void Motor_Set_Angle(u16 Angle)//�趨ת���Ƕ�
{
	
}

u16 Motor_Read_Angle(void)//��ȡ��ǰ�Ѿ�ת���Ƕ�
{
	
}

