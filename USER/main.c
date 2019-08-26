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
#include "function.h"

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

/**
*		2019年8月14日16:36:05：修复在串口中修改pid参数只对速度生效的bug,修复串口指令的诸多BUG 添加setmode指令，现在可以在串口中更改运行模式了 调整角度PID参数为 P
*/

/**
*		2019年8月15日15:15:59：mian.c大改，大段代码移动到单独函数function中，主函数现在更容易阅读了
*/

/**
*		2019年8月15日16:16:26：修复timer.c中定时器中关于不同模式下PID调整选择的代码 修复设定的角度没有上传给单片机的BUG 调整了角度PID参数 在UI界面上添加角度设定指示线
*/

/**
*		2019年8月18日12:10:25：修复串口发送角度在参数-h下数据出错的BUG 修复调节系数后三位由浮点数有效位数导致的数据异常的BUG
*/

/**
*		2019年8月20日12:42:38：更换按键硬件为PCB板印矩阵按键 板子接地后按键一切正常 修改UI界面中速度关于时间的函数图像增加一条线，线宽更粗了
*/

/**
*		2019年8月23日20:01:44：优化了串口发送回的界面，现在看起来更美观了
*/

/**
*		2019年8月26日16:04:40：修改了部分注释，格式化部分代码
*/

/*全局变量定义*/
u8 Dir_rand;//全局变量|电机转向标志位
u8 Mode = 1;//速度|角度调整模式 0：角度 1：速度
u8 key;//按下的按键的键值|主要用于UI上按键键值的显示
int skap = 500;//全局变量|PWM空占比
int setspeed = 70;//全局变量|设定速度值
int setangle = 0;//全局变量|角度设定值
int setangle_buff = 0;//储存确定调节前的角度
float kp = 0.46499,ki = 0.00010,kd = 0.42610;//PID参数 比例|积分|微分0.165 0.00001 0.425
float mp = 1.00000;//PID参数调整系数
char EP_FLAG = 0X00;//EEPROM储存过 标志位


/**
*		函数名：main
*		作用：程序的主体部分，各种模块、参数的初始化，从E2PROM读取数据，刷新、反馈按键
*		入口参数：无
*		返回值：空
*		修改日期：2019年8月11日14:15:47
*/
int main(void)
{
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
	
	InitializeByEEPROM_function();//从EEPROM初始化，注释本段可以打开初始化为原始数据
	
	PID_init(kp,ki,kd,skap);//PID控制初始化|参数-->|比例Kp|积分Ki|微分Kd|空占比skap
	
	delay_ms(200);//延时200ms

	BEEP = 0;//关闭蜂鸣器提示初始化完成
	
	/*主循环|获取键值|限制空占比|失速报警|空占比调整|UI刷新|串口指令读取*/
	while(1)
	{
		key = GetKey();								//获取键值
		Key_function(key);						//针对按下的按键做出反应
		SkipLimit_function();					/*空占比限制|测试得到有效调节范围为999-350*/ /*实际会超过|但是起到了限定不会无限大的作用*/
		SpeedLimi_function();					/*速度限制|规定范围为0-70*/
		SpeedWarning_function();			//误差过大报警，调试的时候注释掉，不然太吵
		Motor_Set_Speed(setspeed);		//速度调整函数
		TIM_SetCompare1(TIM3,skap);		//空占比调整函数
		Select_order();								//读取串口发送过来的命令
		UI_Working();									//用户图形界面更新
		
		if(!Mode)//模式2下更新角度
			Motor_Set_Angle(setangle);
	}
}


