#include "timer.h"
#include "sys.h"

float speed = 0.0;
u8 rand = 0;

void EXTIX_Init(void)//�ⲿ�ж� �����ش���
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //��ʹ�� AFIO ʱ��
	//GPIO A4 �ж����Լ��жϳ�ʼ������,�½��ش���
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure); //�ܳ�ʼ�� EXTI �Ĵ���
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; //ʹ�ܰ����ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ� 2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; //�����ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//�ݳ�ʼ�� NVIC
}

void TIM5_Speed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//��ʼ�� GPIOA.0 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//PA0 ���ü��˿�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//PA0 ��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ�� GPIOA.0
	
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��TIM5ʹ��
	TIM_TimeBaseStructure.TIM_Period = 9999; //�趨�������Զ���װֵ1s
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //��ʼ�� TIM5
	//�ж����ȼ� NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn; //TIM5 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //��ռ���ȼ� 2 ��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //�����ȼ� 1 ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure); //��ʼ�� NVIC �Ĵ���
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //��������ж�
	TIM_Cmd(TIM5, ENABLE); //ʹ�� TIM5
	
	EXTIX_Init();//�ⲿ�жϳ�ʼ��
}

void TIM5_IRQHandler(void)//��ʱ��5 1���ж�
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, DISABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //�����ⲿ�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //��ռ���ȼ� 2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; //�����ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE; //ʧ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//�ݳ�ʼ�� NVIC
	
	speed = rand/334;
	rand = 0;
	
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update); //�����ʱ���ж��жϱ�־λ
}

void EXTI4_IRQHandler(void)
{
	rand++;
	EXTI_ClearITPendingBit(EXTI_Line4); //��� LINE4 �ϵ��жϱ�־λ
}

void TIM3_PWM_Init(u16 arr,u16 psc)//TIM3 PWM��·��� ͨ��һ ��ȫ��ӳ�� �����PA6
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//ʹ�ܶ�ʱ��3ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO,ENABLE);//ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO
 
	
   //��ʼ��TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	//��ʼ��TIM3 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM3 OC1

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
 
	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
	
}
