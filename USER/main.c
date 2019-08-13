#include "stm32f10x.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "timer.h"
#include "motor.h"
#include "usart.h"
#include "pid.h"
#include "beep.h"
#include "menu.h"
#include "lcd.h"
#include "UI.h"
#include "EEPROM.h"
#include "printf.h"

/**
*		2019年7月31日23:10:40：角度是减小的 偶尔速度会和设定值误差较大 设定角度未实现 按键14按下不管用         |        通过按键切换控制模式 设定角度可以旋转
*/

/**
*		2019年8月1日15:14:52：角度问题解决 加入模式转换功能 EEPROM开辟三个内存块用来储存新的PID参数 按键有时候失灵 按键14仍然不管用
*/

/**
*		2019年8月2日13:02:35：将串口菜单放入usart.c中执行 串口指令接收有问题 按键14仍然不管用
*/

/**
*		2019年8月10日22:03:28：将UI.c中的sprintf函数删除 改用自己编写的 float_to_char函数 前几位显示正常 调节系数的后两位显示不正常为 0.00099和0.00009
*													github版本控制开始使用         |        完善代码注释（标准化） float_to_char的修改  控制转过角度实现  通过串口接收指令
*/

/**
*		2019年8月11日14:08:52：float_to_char函数没有问题 错误未知
*/

/**
*		2019年8月12日11:42:38：串口基本完成 数字判定有问题 加入新指令解决 串口指令部分完成 调节系数后两位仍然显示不正常
*/

/**
*		2019年8月13日10:41:15：优化程序结构 添加注释 调整速度PID参数为 P0.46499 I0.00010 D0.42610 角度PID参数为 P1.34309 I0.00000 D0.63900  按键最后一排经常失灵
*/

/*全局变量定义*/
u8 Dir_rand;//全局变量|电机转向标志位
int skap = 500;//全局变量|PWM空占比
int setspeed = 70;//全局变量|设定速度值
int setangle = 0;//全局变量|角度设定值
float kp = 0.165,ki = 0.00001,kd = 0.425;//PID参数 比例|积分|微分0.165 0.00001 0.425

u8 Mode = 1;//速度|角度调整模式 0：角度 1：速度
int setangle_buff = 0;//储存确定调节前的角度
float mp = 1.0;//PID参数调整系数

u8 key;//按下的按键的键值|主要用于UI上按键键值的显示


/*两个功能函数声明*/
void Mode_Switch(u8 mode);//模式切换函数
void Apply(u8 mode);//应用变更函数

/**
*		函数名：main
*		作用：程序的主体部分，各种模块、参数的初始化，从E2PROM读取数据，刷新、反馈按键
*		入口参数：无
*		返回值：空
*		修改日期：2019年8月11日14:15:47
*/

/*主函数*/
int main(void)
{
	int buff;//记录按下翻转键时的空占比,减小电机速度的突变
	char EP_FLAG = 0X00;//EEPROM储存过 标志位
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置NVIC中断分组2|2位抢占优先级，2位响应优先级
	
	BEEP_Init();//蜂鸣器函数初始化
	BEEP = 1;//蜂鸣器鸣叫开启指示正在初始化
	delay_init();//延时函数初始化
	
	uart_init(115200);//串口初始化|波特率115200
	
	KEY_Init();//矩阵按键初始化
	
	Motor_Init();//电机转动\控制\测量初始化
	
	TIM3_PWM_Init(999,0);//定时器3|输出|PWM输出|999重装值比较合适(尝试)
	TIM4_PID_Init(9999,71);//定时器4|中断|PID调整控制初始化
	TIM5_Cap_Init(0xFFFF,71);//定时器5|定时\捕获中断|电机测速
	
	Dir_rand = Motor_dir(1);//默认电机正传
		
	LCD_Init();//初始化LCD屏幕
	LCD_Clear(YELLOW);//设定背景颜色为黄色
	
	EEPROM_Init();//E2PROM初始化|IIC总线
	
	/*从EEPROM初始化*/
	EP_FLAG = EEPROM_Read(0x1B);//获取标志位,根据标志位初始化
	if(( EP_FLAG&0x01) == 0x01) setspeed = EEPROM_Read(0x00);//初始化速度
	if(( EP_FLAG&0x02) == 0x02) setangle_buff = EEPROM_Read(0x01);//初始化角度
	if(( EP_FLAG&0x04) == 0x04) Dir_rand = Motor_dir(EEPROM_Read(0x02));//初始化方向
	if(Mode)//从EEPROM PID参数（速度）初始化PID参数
	{
		if(( EP_FLAG&0x08) == 0x08) kp = EEPROM_ReadFloat(0x03);//初始化kp
		if(( EP_FLAG&0x10) == 0x10) ki = EEPROM_ReadFloat(0x0B);//初始化ki
		if(( EP_FLAG&0x20) == 0x20) kd = EEPROM_ReadFloat(0x13);//初始化kd
	}else//从EEPROM PID参数（角度）初始化PID参数
	{
		setspeed = 0;
		if(( EP_FLAG&0x08) == 0x08) kp = EEPROM_ReadFloat(0x23);//初始化kp
		if(( EP_FLAG&0x10) == 0x10) ki = EEPROM_ReadFloat(0x2B);//初始化ki
		if(( EP_FLAG&0x20) == 0x20) kd = EEPROM_ReadFloat(0x33);//初始化kd
	}
	//注释本段可以打开初始化为原始数据
	
	PID_init(kp,ki,kd,skap);//PID控制初始化|参数-->|比例Kp|积分Ki|微分Kd|空占比skap
	
	delay_ms(200);//延时200ms

	BEEP = 0;//关闭蜂鸣器提示初始化完成
	/*主循环|获取键值|限制空占比|失速报警|空占比调整|UI刷新|串口指令读取*/
	while(1)
	{
		
		key = GetKey();//获取键值
		
		switch(key)//按键反馈
		{
			case 1:setspeed++;EEPROM_Write(0x00,setspeed);EP_FLAG |= 0x01;EEPROM_Write(0x1B,EP_FLAG);delay_ms(50);break;
			//增加设定的速度|每次增加都将刷新EEPROM的对应内存地址
			case 2:setangle_buff++;EEPROM_Write(0x01,setangle_buff);EP_FLAG |= 0x02;EEPROM_Write(0x1B,EP_FLAG);delay_ms(50);break;
			//增加设定的角度|每次增加都将刷新EEPROM的对应内存地址
			case 3:buff = skap;skap = 0;delay_ms(100);skap = buff;Dir_rand = Motor_dir(1);EEPROM_Write(0x02,Dir_rand);EP_FLAG |= 0x04;EEPROM_Write(0x1B,EP_FLAG);break;
			//设置电机为正传|每次按下都将刷新EEPROM的对应内存地址
			case 4:Apply(Mode);delay_ms(50);break;
			//应用更改|模式1下应用对 PID参数 的更改|模式2下应用对 角度|PID参数 的更改
			case 5:setspeed--;EEPROM_Write(0x00,setspeed);EP_FLAG |= 0x01;delay_ms(50);EEPROM_Write(0x1B,EP_FLAG);break;
			//降低设定的速度|每次降低都将刷新EEPROM的对应内存地址
			case 6:setangle_buff--;EEPROM_Write(0x01,setangle_buff);EP_FLAG |= 0x02;delay_ms(50);EEPROM_Write(0x1B,EP_FLAG);break;
			//降低设定的角度|每次降低都将刷新EEPROM的对应内存地址
			case 7:buff = skap;skap = 0;delay_ms(100);skap = buff;Dir_rand = Motor_dir(0);EEPROM_Write(0x02,Dir_rand);EP_FLAG |= 0x04;EEPROM_Write(0x1B,EP_FLAG);break;
			//设置电机为反|每次按下都将刷新EEPROM的对应内存地址
			case 8:if(mp>0.00001) mp *= 0.1;else mp = 1.0;delay_ms(50);break;
			//调整PID调整系数|每次按下减小1倍
			case 9:kp += 1 * mp;if(Mode){EEPROM_WriteFloat(0x03,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x23,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//根据调整系数增加KP|根据模式存放数据于对应地址
			case 10:Mode = !Mode;Mode_Switch(Mode);delay_ms(100);break;
			//设定 速度|角度 模式切换	
			case 11:ki += 1 * mp;if(Mode){EEPROM_WriteFloat(0x0B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x2B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//根据调整系数增加KI|根据模式存放数据于对应地址
			case 12:kd += 1 * mp;if(Mode){EEPROM_WriteFloat(0x13,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x33,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//根据调整系数增加KD|根据模式存放数据于对应地址
			case 13:kp -= 1 * mp;if(Mode){EEPROM_WriteFloat(0x03,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x23,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//根据调整系数减少KP|根据模式存放数据于对应地址
			case 14:/*保留*/;break;
			//保留|此按键损坏
			case 15:ki -= 1 * mp;if(Mode){EEPROM_WriteFloat(0x0B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x2B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//根据调整系数减少KI|根据模式存放数据于对应地址
			case 16:kd -= 1 * mp;if(Mode){EEPROM_WriteFloat(0x13,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x33,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//根据调整系数减少KD|根据模式存放数据于对应地址
			default:break;
		}
		
		/*空占比限制|测试得到有效调节范围为999-350*/
		/*实际会超过|但是起到了限定不会无限大的作用*/
		if(skap>999)
		{
			skap = 999;
		}else if(skap<350)
		{
			skap = 350;
		}
		
		/*速度限制|规定范围为0-70*/
		/*之前是25-70|因为调整角度时速度可以达到0，故更改，但是实际速度范围仍然为25-30*/
		if(setspeed>70)
		{
			setspeed = 70;
		}else if(setspeed<0)
		{
			setspeed = 0;
		}
		
		
		
		/*误差过大报警*/
		if((setspeed-Motor_Read_Speed()>20||setspeed-Motor_Read_Speed()<-20) && Mode)//误差超过20并且在模式1下报警
		{
			BEEP = 1;
		}else 
		{
			BEEP = 0;
		}//调试的时候注释掉
		
		Motor_Set_Speed(setspeed);//速度调整函数
		
		TIM_SetCompare1(TIM3,skap);//空占比调整函数
		
		Select_order();//读取串口发送过来的命令
		
		UI_Working();//用户图形界面更新
		
		
		if(!Mode)//模式2下更新角度
			Motor_Set_Angle(setangle);
	}
}

/**
*		函数名：Mode_Switch
*		作用：在设定速度和设定角度模式中进行转换
*		入口参数：uchar型模式标志
*		返回值：无
*		修改日期：2019年8月11日14:11:25
*/

/**
*		模式选择函数
*		0：模式2 1：模式1
**********************/
void Mode_Switch(u8 mode)
{
	if(mode)
	{
		setspeed = EEPROM_Read(0x00);
		kp = EEPROM_ReadFloat(0x03);//初始化kp
		ki = EEPROM_ReadFloat(0x0B);//初始化ki
		kd = EEPROM_ReadFloat(0x13);//初始化kd
		PID_init(kp,ki,kd,skap);//PID控制初始化|参数-->|比例Kp|积分Ki|微分Kd|空占比skap
	}else
	{
		setspeed = 0;
		kp = EEPROM_ReadFloat(0x23);//初始化kp
		ki = EEPROM_ReadFloat(0x2B);//初始化ki
		kd = EEPROM_ReadFloat(0x33);//初始化kd
		PID_init(kp,ki,kd,skap);//PID控制初始化|参数-->|比例Kp|积分Ki|微分Kd|空占比skap
	}
}

/**
*		函数名：Apply
*		作用：不同模式下 应用对PID参数、角度设定的变更
*		入口参数：uchar型模式标志
*		返回值：无
*		修改日期：2019年8月11日14:13:37
*/

/**
*		应用变更
*		模式2下应用 PID参数|速度上传；模式1下应用PID参数
**************************************************/
void Apply(u8 mode)
{
	if(mode)
	{
		kp = EEPROM_ReadFloat(0x03);//初始化kp
		ki = EEPROM_ReadFloat(0x0B);//初始化ki
		kd = EEPROM_ReadFloat(0x13);//初始化kd
		PID_init(kp,ki,kd,skap);//PID控制初始化|参数-->|比例Kp|积分Ki|微分Kd|空占比skap
	}else
	{
		setangle = setangle_buff;
		kp = EEPROM_ReadFloat(0x23);//初始化kp
		ki = EEPROM_ReadFloat(0x2B);//初始化ki
		kd = EEPROM_ReadFloat(0x33);//初始化kd
		PID_init(kp,ki,kd,skap);//PID控制初始化|参数-->|比例Kp|积分Ki|微分Kd|空占比skap
	}
}
