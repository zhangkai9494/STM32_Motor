#include "timer.h"
#include "sys.h"
#include "pid.h"
#include "motor.h"
#include "usart.h"

/**
*		TIM3 通道一 PWM输出
*		IO口：PA6
*		main函数中初始化的参数为 TIM3_PWM_Init(999,0)
*		不分频 重装值为999
*/

void TIM3_PWM_Init(u16 arr,u16 psc)//TIM3 PWM单路输出 通道一 完全不映射 输出口PA6
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA| RCC_APB2Periph_AFIO,ENABLE);//使能GPIOA外设和AFIO复用功能模块时钟
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//在PA6上输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIO
 
	
  //初始化TIM3
	TIM_TimeBaseStructure.TIM_Period = arr;//自动重装载寄存器值
	TIM_TimeBaseStructure.TIM_Prescaler =psc;//预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);//初始化
	
	//初始化TIM3 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//PWM模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//低电平输出
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);//初始化外设TIM3 OC1
 
	TIM_Cmd(TIM3, ENABLE);  //使能TIM3
	
}

/**
*		TIM5 通道一 外部捕获|定时器
*		捕获IO口：PA0		定时器周期：65535us
*		测速方式为定角度测时间后换算得到转每秒
*   main函数初始化为 TIM5_Cap_Init(0xFFFF,71)
*		72分频 重装再值为最大65535
*/

void TIM5_Cap_Init(u16 arr,u16 psc)//定时器5 通道1 输入捕获 上升沿捕获 定时器65535us溢出一次
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM5_ICInitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);//使能TIM5时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;//PA0 初始化  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//PA0 下拉输入  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA,GPIO_Pin_0); //PA0 下拉
	
	//初始化定时器5 TIM5	 
	TIM_TimeBaseStructure.TIM_Period = arr;//设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Prescaler =psc;//预分频器
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
  
	//初始化TIM5通道一输入捕获参数
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //通道一 PA0引脚捕获
  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//上升沿捕获
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//映射到TI1上|默认不更改
 	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;//不分频 
 	TIM5_ICInitStructure.TIM_ICFilter = 0x00;//不滤波
  TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	
	//初始化TIM5通道二输入捕获参数
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; //通道二 PA1引脚捕获
  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;//上升沿捕获
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;//映射到TI1上|默认不更改
 	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;//不分频 
 	TIM5_ICInitStructure.TIM_ICFilter = 0x00;//不滤波
  TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;//TIM5中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//2抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//0响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1|TIM_IT_CC2,ENABLE);//允许更新中断 ,允许通道一捕获中断	
	
  TIM_Cmd(TIM5,ENABLE ); //使能定时器5
		
}

/**
*   进入事件有两个 定时器溢出或者捕获到上升沿
*   定时器溢出处理 溢出计数器加一 清零中断标志位
*   捕获到上升沿处理 获取溢出计数器的值 获取定时器的当前值 转过 1.07°的时间为 T=溢出计数器值*65536+当前定时器值 则转过一圈的时间为 t=334*T
*		速度就为 V=1,000,000/t 转每秒（rad/s）
*		变量定义 定时器溢出计数变量TIM_Full 转过一个角度用的时间Time 速度Speed
*		速度为外部全局变量 在<motor.c>中包含 TIM_Full和Time在出捕获中断时进行清零
*		偶尔出现异常测速值
*/

/*全局变量定义*/
float Speed = 0.0;//速度输出
u16 TIM_Full = 0;//定时器溢出计数器
u16 Time = 0;//定时器计时值获取
u16 angle = 0;
 
//定时器5中断服务程序	 
void TIM5_IRQHandler(void)
{
	
	if(TIM_GetITStatus(TIM5, TIM_IT_Update)==SET)//如果是定时器中断
	{
		TIM_Full++;//溢出计数器加一
		
		/*多次溢出达到上限后说明电机已经停止转动*/
		if(TIM_Full>=2)
		{
			TIM_Full = 0;//清除定时器溢出计数器
			Speed = 0;//返回0速度
		}
	}
	
	if(TIM_GetITStatus(TIM5, TIM_IT_CC1)==SET)//捕获1发生捕获事件
	{
		Time=TIM_GetCapture1(TIM5);//获取当前定时器的值
		Time += TIM_Full*65536;
		Time *=334;
		Speed = 1000000.0/Time;//得到速度
		Time = 0;
		TIM_Full = 0;
		TIM_SetCounter(TIM5,0);//清零计时器
	}
	
	if(TIM_GetITStatus(TIM5, TIM_IT_CC2)==SET)//捕获2发生捕获事件|测量走过的角度数（实际工作中采样率不够，变化看起来不连续）
	{
		angle++;
		if(angle>360)
			angle=0;
	}
	
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC1|TIM_IT_CC2|TIM_IT_Update); //清除中断标志位
	
}


/**
*		TIM4 未引用IO口 定时器
*		定时器中断打开 中断发生后进行PID调整
*		main函数初始化为 TIM4_PID_Init(9999,71)
*		72分频 溢出值为10000 定时时间为 10ms
*/
void TIM4_PID_Init(u16 arr,u16 psc)//定时器4|PID调整用
{
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//使能定时器4时钟
 
  //初始化TIM4
	TIM_TimeBaseStructure.TIM_Period = arr;//重装值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;//TIM4中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = TIM_CKD_DIV1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
 
	TIM_Cmd(TIM4, ENABLE);  //使能TIM4
	
}

void TIM4_IRQHandler(void)//定时器4中断服务函数|根据当前速度由pid算法得到当前空占比
{
	extern int skap;
	extern int setspeed;
	extern int setangle;
	extern u8 Mode;
	
	if(!Mode)//角度PID调整模式
		skap += PID_Angle(setangle,Motor_Read_Angle());
	else
		skap += PID_realize(setspeed,Motor_Read_Speed());
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
}
