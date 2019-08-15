#include "UI.h"
#include "lcd.h"
#include "motor.h"

/*全局变量定义|每次进入这些值不希望被初始化*/
int x=5,y =150;//速度时间图像本次点
int last_x = 5,last_y = 150;//速度时间图像上次点
int last_ay = 280;//角度时间图像上次点
int last_sety = 150;
char kp0[8],ki0[8],kd0[8];//8位数组存放PID参数转化为Char型的值
char mp0[8];//存放PID参数调整系数转化为Char型数组后的值

int angle_y = 279;

extern u8 Mode;

/**
*		函数名：float_to_char
*		作用：将小数点后5位的float型数据变为char型储存在传入的数组中
*		入口参数：float型的原始数据 8个元素的char型数组
*		返回值：无
*		修改日期：2019年8月10日21:24:48
*/

void float_to_char(float number,char buff[8])
{
	buff[0] = (int)number%10+48;
	buff[1] = '.';
	buff[2] = (int)(number*10)%10+48;
	buff[3] = (int)(number*100)%10+48;
	buff[4] = (int)(number*1000)%10+48;
	buff[5] = (int)(number*10000)%10+48;
	buff[6] = (int)(number*100000)%10+48;
	buff[7] = '\0';
}

/**
*		UI刷新函数
*		页面元素：
*		线框1			容纳  Setspeed Setangle 和 方向提示符
*		线框2			容纳	Speed Angle Skap
*		线框3			容纳	动态图像 目标速度线 当前速度关于时间的函数图像
*		线框4			容纳	动态图像 当前角度关于时间的函数图像
*		线框5			容纳	Kp Ki Kd Key MP 误差过大报警提示符
*/

void UI_Working(void)
{
	/*外部变量定义*/
	extern int setspeed;//速度设定值
	extern int setangle_buff;//角度设定值
	extern int skap;
	extern u8 Dir_rand;//转向标志符
	extern u8 key;//按键获取值
	extern float kp,ki,kd;//PID参数
	extern float mp;
	
	/*将float型参数转化为char型储存在数组中*/
	float_to_char(kp,kp0);
	float_to_char(ki,ki0);
	float_to_char(kd,kd0);
	float_to_char(mp,mp0);
	
	
	/*画线框*/
	POINT_COLOR=BLUE;
	LCD_DrawRectangle(5,5,235,30);//线框1
	
	POINT_COLOR=BLACK;
	LCD_DrawRectangle(5,35,235,60);//线框2
	
	POINT_COLOR=MAGENTA;
	LCD_DrawRectangle(5,65,235,155);//线框3
	
	POINT_COLOR=MAGENTA;
	LCD_DrawRectangle(5,160,235,280);//线框4
	
	POINT_COLOR=BLUE;
	LCD_DrawRectangle(5,285,135,310);//线框5
	
	
	/*填充数据*/
	POINT_COLOR=RED;
	LCD_ShowString(10,10,200,16,16,"Setspeed:");
	LCD_ShowxNum(80,10,setspeed,3,16,0);
	LCD_ShowString(110,10,200,16,16,"Setangle:");
	LCD_ShowxNum(180,10,setangle_buff,3,16,0);
	
	if(Dir_rand==0)
		LCD_ShowChar(215,10,82,16,0);
	else
		LCD_ShowChar(215,10,76,16,0);
	
	LCD_ShowString(10,40,200,16,16,"Speed:");
	LCD_ShowxNum(58,40,Motor_Read_Speed(),3,16,0);
	LCD_ShowString(86,40,200,16,16,"Angle:");
	LCD_ShowxNum(132,40,Motor_Read_Angle(),3,16,0);
	LCD_ShowString(160,40,200,16,16,"Skap:");
	LCD_ShowxNum(200,40,skap,4,16,0);
	
	LCD_ShowString(7,286,200,12,12,"Kp:");
	LCD_ShowString(25,286,200,12,12,kp0);
	LCD_ShowString(7,298,200,12,12,"Ki:");
	LCD_ShowString(25,298,200,12,12,ki0);
	LCD_ShowString(75,286,200,12,12,"Kd:");
	LCD_ShowString(93,286,200,12,12,kd0);
	POINT_COLOR=BLUE;
	LCD_ShowString(82,298,200,12,12,"Key:");
	LCD_ShowNum(106,298,key,2,12);
	POINT_COLOR=BLACK;
	LCD_ShowString(142,286,200,12,12,"MP:");
	LCD_ShowString(160,286,200,12,12,mp0);
	
	/*速度\角度对于时间的函数图像绘制*/
	POINT_COLOR=BLACK;
	if(x<235)
	{
		x++;
		/*限制绘画边界*/
		if(150-Motor_Read_Speed()<67)
			y = 67;
		else if(150-Motor_Read_Speed()>150)
			y = 150;
		else
			y = 150-Motor_Read_Speed();
		
		/*速度画线|双线*/
		LCD_DrawLine(last_x,last_y,x,y);
		LCD_DrawLine(last_x,last_y-1,x,y-1);
		last_x = x;
		last_y = y;
		
		/*角度画线|双线*/
		LCD_DrawLine(last_x,last_ay,x,280-(int)Motor_Read_Angle()/3.3);
		last_ay = 280-(int)Motor_Read_Angle()/3.3;
		
		/*画角度出现问题 保留调整*/
		/*采样率不够数据重叠导致*/
		
	}else//如果横向超出绘制边框
	{
		LCD_Fill(5,65,235,155,YELLOW);//清屏
		LCD_Fill(5,160,235,280,YELLOW);
		
		POINT_COLOR=MAGENTA;
		LCD_DrawRectangle(5,65,235,155);
	
		POINT_COLOR=MAGENTA;
		LCD_DrawRectangle(5,160,235,280);
		
		/*初始化横坐标值*/
		last_x = 5;
		x = 5;
	}
	
	/*速度指示线绘制*/
	POINT_COLOR=YELLOW;
	LCD_DrawLine(5,last_sety,235,last_sety);
	POINT_COLOR=GREEN;
	LCD_DrawLine(5,150-setspeed,235,150-setspeed);
	last_sety = 150-setspeed;
	
	/*角度指示线绘制*/
	POINT_COLOR=YELLOW;
	LCD_DrawLine(5,angle_y,235,angle_y);
	POINT_COLOR=GREEN;
	LCD_DrawLine(5,279-setangle_buff/3.3,235,279-setangle_buff/3.3);
	angle_y = 279-setangle_buff/3.3;
	
	/*误差过大报警标志|误差超过20*/
	if(setspeed-Motor_Read_Speed()>20||setspeed-Motor_Read_Speed()<-20)//误差超过20的报警
	{
		POINT_COLOR=RED;
		LCD_ShowString(145,298,200,12,12,"Warning!");
	}else 
	{
		LCD_Fill(144,297,235,310,YELLOW);
	}
	
	/*模式指示*/
	POINT_COLOR=RED;
	if(Mode)
		LCD_ShowChar(210,286,83,24,0);
	else
		LCD_ShowChar(210,286,65,24,0);
	
	
}

