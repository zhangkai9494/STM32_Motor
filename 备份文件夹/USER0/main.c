#include "stm32f10x.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "motor.h"
#include "usart.h"
#include "pid.h"

int main(void)
{
	int key,buff;
	int skap = 500;
	
	extern float speed;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	delay_init();
  LED_Init();
	
	uart_init(115200);
	
	KEY_Init();
	TIM3_PWM_Init(499,250);
	
	Motor_Init();
	Motor_dir(1);
	
	//TIM5_Speed();//函数有问题 无法跳出；

	while(1)
	{
		key = GetKey();
		printf("%d",key);
		switch(key)
		{
			case 1:skap++;break;
			case 2:skap--;break;
			case 3:skap = 0;break;
			case 4:skap = 499;break;
			case 5:buff = skap;skap = 0;delay_ms(100);skap = buff;Motor_dir(1);break;
			case 6:buff = skap;skap = 0;delay_ms(100);skap = buff;Motor_dir(0);break;
			default:break;
		}
		if(skap>499)
		{
			skap = 499;
		}
		if(skap<0)
		{
			skap = 0;
		}
		TIM_SetCompare1(TIM3,skap);
	}
}
