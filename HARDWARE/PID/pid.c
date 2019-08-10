#include "pid.h"
#include "beep.h"
#include "delay.h"

/*PID�����ṹ��*/
struct{
    float SetSpeed;//�趨�ٶȣ��Ӻ�����ڲ������ݣ�
    float ActualSpeed;//��ȡ����ʵ���ٶȣ�ʵʱ������Ϊ��ڲ������ݣ�
    float err;//�������ĵõ���ƫ��ֵ
    float err_last;//��һ��ƫ��ֵ����Դ����һ�εĺ������� ����Ϊ��̬�������ɣ�
    float Kp,Ki,Kd;//������������֡�΢��ϵ��
    float integral;//�������ֵ
    float voltage;//�������õ���˲��PWMֵ
}pid;


/*PID��ʼ��*/
void PID_init(float Kp,float Ki,float Kd,u16 skap)//PID��ʼ������ ��ʼ��ʱ���� ��������΢��ϵ�� ��ǰPWM��ռ�� 
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

/*PID�㷨ʵ��*/
/*����ֵΪ��ǰ����¿�ռ�ȵĵ�����*/
int PID_realize(float speed,float ActualSpeed)//�趨�ٶ�|ʵ���ٶȣ�����ʵʱ��ȡ��
{
	pid.ActualSpeed=ActualSpeed;
	pid.SetSpeed=speed;
	pid.err=pid.SetSpeed-pid.ActualSpeed;
	pid.integral+=pid.err;
	pid.voltage=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);
	pid.err_last=pid.err;
	
	return pid.voltage;//������Ҫ��PWM��ռ��
}

int PID_Angle(float setangle,float angle)//�趨�ٶ�|ʵ���ٶȣ�����ʵʱ��ȡ��
{
	pid.ActualSpeed=angle;
	pid.SetSpeed=setangle;
	pid.err=pid.SetSpeed-pid.ActualSpeed;
	pid.integral+=pid.err;
	pid.voltage=pid.Kp*pid.err+pid.Ki*pid.integral+pid.Kd*(pid.err-pid.err_last);
	pid.err_last=pid.err;
	
	return pid.voltage;//������Ҫ��PWM��ռ��
}
