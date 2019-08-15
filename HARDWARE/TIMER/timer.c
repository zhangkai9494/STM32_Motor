#include "timer.h"
#include "sys.h"
#include "pid.h"
#include "motor.h"
#include "usart.h"

/**
*		TIM3 ͨ��һ PWM���
*		IO�ڣ�PA6
*		main�����г�ʼ���Ĳ���Ϊ TIM3_PWM_Init(999,0)
*		����Ƶ ��װֵΪ999
*/

void TIM3_PWM_Init(u16 arr,u16 psc)//TIM3 PWM��·��� ͨ��һ ��ȫ��ӳ�� �����PA6
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO,ENABLE);//ʹ��GPIOA�����AFIO���ù���ģ��ʱ��
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//��PA6�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
 
	
  //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr;//�Զ���װ�ؼĴ���ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc;//Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);//��ʼ��
	
	//��ʼ��TIM3 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//PWMģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//�͵�ƽ���
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);//��ʼ������TIM3 OC1
 
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
	
}

/**
*		TIM5 ͨ��һ �ⲿ����|��ʱ��
*		����IO�ڣ�PA0		��ʱ�����ڣ�65535us
*		���ٷ�ʽΪ���ǶȲ�ʱ�����õ�תÿ��
*   main������ʼ��Ϊ TIM5_Cap_Init(0xFFFF,71)
*		72��Ƶ ��װ��ֵΪ���65535
*/

void TIM5_Cap_Init(u16 arr,u16 psc)//��ʱ��5 ͨ��1 ���벶�� �����ز��� ��ʱ��65535us���һ��
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM5_ICInitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//ʹ��TIM5ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0 ��ʼ��  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//PA0 ��������  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0); //PA0 ����
	
	//��ʼ����ʱ��5 TIM5	 
	TIM_TimeBaseStructure.TIM_Period = arr;//�趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc;//Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
  
	//��ʼ��TIM5ͨ��һ���벶�����
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //ͨ��һ PA0���Ų���
  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//�����ز���
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//ӳ�䵽TI1��|Ĭ�ϲ�����
 	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;//����Ƶ 
 	TIM5_ICInitStructure.TIM_ICFilter = 0x00;//���˲�
  TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	
	//��ʼ��TIM5ͨ�������벶�����
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; //ͨ���� PA1���Ų���
  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//�����ز���
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//ӳ�䵽TI1��|Ĭ�ϲ�����
 	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;//����Ƶ 
 	TIM5_ICInitStructure.TIM_ICFilter = 0x00;//���˲�
  TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;//TIM5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//2��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//0��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2,ENABLE);//��������ж� ,����ͨ��һ�����ж�	
	
  TIM_Cmd(TIM5,ENABLE ); //ʹ�ܶ�ʱ��5
		
}

/**
*   �����¼������� ��ʱ��������߲���������
*   ��ʱ��������� �����������һ �����жϱ�־λ
*   ���������ش��� ��ȡ�����������ֵ ��ȡ��ʱ���ĵ�ǰֵ ת�� 1.07���ʱ��Ϊ T=���������ֵ*65536+��ǰ��ʱ��ֵ ��ת��һȦ��ʱ��Ϊ t=334*T
*		�ٶȾ�Ϊ V=1,000,000/t תÿ�루rad/s��
*		�������� ��ʱ�������������TIM_Full ת��һ���Ƕ��õ�ʱ��Time �ٶ�Speed
*		�ٶ�Ϊ�ⲿȫ�ֱ��� ��<motor.c>�а��� TIM_Full��Time�ڳ������ж�ʱ��������
*		ż�������쳣����ֵ
*/

/*ȫ�ֱ�������*/
float Speed = 0.0;//�ٶ����
u16 TIM_Full = 0;//��ʱ�����������
u16 Time = 0;//��ʱ����ʱֵ��ȡ
u16 angle = 0;
 
//��ʱ��5�жϷ������	 
void TIM5_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM5, TIM_IT_Update)==SET)//����Ƕ�ʱ���ж�
	{
		TIM_Full++;//�����������һ
		
		/*�������ﵽ���޺�˵������Ѿ�ֹͣת��*/
		if(TIM_Full>=2)
		{
			TIM_Full = 0;//�����ʱ�����������
			Speed = 0;//����0�ٶ�
		}
	}
	
	if(TIM_GetITStatus(TIM5, TIM_IT_CC1)==SET)//����1���������¼�
	{
		Time=TIM_GetCapture1(TIM5);//��ȡ��ǰ��ʱ����ֵ
		Time += TIM_Full*65536;
		Time *=334;
		Speed = 1000000.0/Time;//�õ��ٶ�
		Time = 0;
		TIM_Full = 0;
		TIM_SetCounter(TIM5,0);//�����ʱ��
	}
	
	if(TIM_GetITStatus(TIM5, TIM_IT_CC2)==SET)//����2���������¼�|�����߹��ĽǶ�����ʵ�ʹ����в����ʲ������仯��������������
	{
		angle++;
		if(angle>360)
			angle=0;
	}
	
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_Update); //����жϱ�־λ
	
}


/**
*		TIM4 δ����IO�� ��ʱ��
*		��ʱ���жϴ� �жϷ��������PID����
*		main������ʼ��Ϊ TIM4_PID_Init(9999,71)
*		72��Ƶ ���ֵΪ10000 ��ʱʱ��Ϊ 10ms
*/
void TIM4_PID_Init(u16 arr,u16 psc)//��ʱ��4|PID������
{
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//ʹ�ܶ�ʱ��4ʱ��
 
  //��ʼ��TIM4
	TIM_TimeBaseStructure.TIM_Period = arr;//��װֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//�жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;//TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM_CKD_DIV1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ��� 
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
 
	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM4
	
}

void TIM4_IRQHandler(void)//��ʱ��4�жϷ�����|���ݵ�ǰ�ٶ���pid�㷨�õ���ǰ��ռ��
{
	extern int skap;
	extern int setspeed;
	extern int setangle;
	extern u8 Mode;
	
	if(!Mode)//�Ƕ�PID����ģʽ
		skap += PID_Angle(setangle,Motor_Read_Angle());
	else
		skap += PID_realize(setspeed,Motor_Read_Speed());
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}
