#include "pid.h"
#include "beep.h"
#include "delay.h"

/*PID参数结构体*/
struct{
    float SetSpeed;//设定速度（从函数入口参数传递）
    float ActualSpeed;//读取到的实际速度（实时测速作为入口参数传递）
    float err;//保存计算的得到的偏差值
    float err_last;//上一个偏差值（来源于上一次的函数计算 定义为静态变量即可）
    float Kp,Ki,Kd;//定义比例、积分、微分系数
    float integral;//定义积分值
    float voltage;//储存计算得到的瞬间PWM值
}pid;


/*PID初始化*/
void PID_init(float Kp,float Ki,float Kd,u16 skap)//PID初始化函数 初始化时传入 比例积分微分系数 当前PWM空占比 
{
	BEEP_Init();
	delay_init();
	
	BEEP = 1;
	
	pid.SetSpeed=0.0;
	pid.ActualSpeed=0.0;
	pid.err=0.0;
	pid.err_last=0.0;
	pid.voltage=skap;
	pid.integral=0.0;
	pid.Kp=Kp;
	pid.Ki=Ki;
	pid.Kd=Kd;
	
	delay_ms(50);
	
	BEEP = 0;
}

/*PID算法实现*/
/*返回值为当前情况下空占比的调整量*/
int PID_realize(float speed,float ActualSpeed)//设定速度|实际速度（测速实时读取）
{
	pid.ActualSpeed=ActualSpeed;
	pid.SetSpeed=speed;
	pid.err=pid.SetSpeed-pid.ActualSpeed;
	pid.integral+=pid.err;
	pid.voltage=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);
	pid.err_last=pid.err;
	
	return pid.voltage;//返回需要的PWM空占比
}

int PID_Angle(float setangle,float angle)//设定速度|实际速度（测速实时读取）
{
	pid.ActualSpeed=angle;
	pid.SetSpeed=setangle;
	pid.err=pid.SetSpeed-pid.ActualSpeed;
	pid.integral+=pid.err;
	pid.voltage=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);
	pid.err_last=pid.err;
	
	return pid.voltage;//返回需要的PWM空占比
}
