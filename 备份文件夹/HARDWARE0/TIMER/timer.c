#include "timer.h"
#include "sys.h"

float speed = 0.0;
u8 rand = 0;

void EXTIX_Init(void)//外部中断 上升沿触发
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); //②使能 AFIO 时钟
	//GPIO A4 中断线以及中断初始化配置,下降沿触发
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
	EXTI_InitStructure.EXTI_Line=EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStructure); //④初始化 EXTI 寄存器
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; //使能按键外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级 2，
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; //子优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//⑤初始化 NVIC
}

void TIM5_Speed(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//初始化 GPIOA.0 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//PA0 设置检测端口
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//PA0 下拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化 GPIOA.0
	
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟TIM5使能
	TIM_TimeBaseStructure.TIM_Period = 9999; //设定计数器自动重装值1s
	TIM_TimeBaseStructure.TIM_Prescaler = 7199; //预分频器
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; // TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM 向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //初始化 TIM5
	//中断优先级 NVIC 设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn; //TIM5 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //先占优先级 2 级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; //从优先级 1 级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ 通道被使能
	NVIC_Init(&NVIC_InitStructure); //初始化 NVIC 寄存器
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //允许更新中断
	TIM_Cmd(TIM5, ENABLE); //使能 TIM5
	
	EXTIX_Init();//外部中断初始化
}

void TIM5_IRQHandler(void)//定时器5 1秒中断
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, DISABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //按键外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级 2，
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; //子优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE; //失能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//⑤初始化 NVIC
	
	speed = rand/334;
	rand = 0;
	
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update); //清除定时器中断中断标志位
}

void EXTI4_IRQHandler(void)
{
	rand++;
	EXTI_ClearITPendingBit(EXTI_Line4); //清除 LINE4 上的中断标志位
}

void TIM3_PWM_Init(u16 arr,u16 psc)//TIM3 PWM单路输出 通道一 完全不映射 输出口PA6
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO,ENABLE);//使能GPIO外设和AFIO复用功能模块时钟
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
	
   //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM3 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM3 OC1

	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
 
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
	
}
