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
*		2019��7��31��23:10:40���Ƕ��Ǽ�С�� ż���ٶȻ���趨ֵ���ϴ� �趨�Ƕ�δʵ�� ����14���²�����         |        ͨ�������л�����ģʽ �趨�Ƕȿ�����ת
*/
/**
*		2019��8��1��15:14:52���Ƕ������� ����ģʽת������ EEPROM���������ڴ�����������µ�PID���� ������ʱ��ʧ�� ����14��Ȼ������
*/
/**
*		2019��8��2��13:02:35�������ڲ˵�����usart.c��ִ�� ����ָ����������� ����14��Ȼ������
*/

/*ȫ�ֱ�������*/
u8 Dir_rand;//ȫ�ֱ���|���ת���־λ
int skap = 500;//ȫ�ֱ���|PWM��ռ��
int setspeed = 70;//ȫ�ֱ���|�趨�ٶ�ֵ
int setangle = 0;//ȫ�ֱ���|�Ƕ��趨ֵ
float kp = 0.165,ki = 0.00001,kd = 0.425;//PID���� ����|����|΢��0.165 0.00001 0.425

u8 Mode = 1;//�ٶ�|�Ƕȵ���ģʽ 0���Ƕ� 1���ٶ�
int setangle_buff = 0;//����ȷ������ǰ�ĽǶ�
float mp = 1.0;//PID��������ϵ��

u8 key;//���µİ����ļ�ֵ|��Ҫ����UI�ϰ�����ֵ����ʾ



void Mode_Switch(u8 mode);//ģʽ�л�����
void Apply(u8 mode);//Ӧ�ñ������



/*������*/
int main(void)
{
	int buff;//��¼���·�ת��ʱ�Ŀ�ռ��,��С����ٶȵ�ͻ��
	char EP_FLAG = 0X00;//EEPROM����� ��־λ
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����NVIC�жϷ���2|2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	
	BEEP_Init();//������������ʼ��
	BEEP = 1;//���������п���ָʾ���ڳ�ʼ��
	delay_init();//��ʱ������ʼ��
	
	uart_init(115200);//���ڳ�ʼ��|������115200
	
	KEY_Init();//���󰴼���ʼ��
	
	Motor_Init();//���ת��\����\������ʼ��
	
	TIM3_PWM_Init(999,0);//��ʱ��3|���|PWM���|999��װֵ�ȽϺ���(����)
	TIM4_PID_Init(9999,71);//��ʱ��4|�ж�|PID�������Ƴ�ʼ��
	TIM5_Cap_Init(0xFFFF,71);//��ʱ��5|��ʱ\�����ж�|�������
	
	Dir_rand = Motor_dir(1);//Ĭ�ϵ������
		
	LCD_Init();//��ʼ��LCD��Ļ
	LCD_Clear(YELLOW);//�趨������ɫΪ��ɫ
	
	EEPROM_Init();//E2PROM��ʼ��|IIC����
	
	/*��EEPROM��ʼ��*/
	EP_FLAG = EEPROM_Read(0x1B);//��ȡ��־λ,���ݱ�־λ��ʼ��
	if(( EP_FLAG&0x01) == 0x01) setspeed = EEPROM_Read(0x00);//��ʼ���ٶ�
	if(( EP_FLAG&0x02) == 0x02) setangle_buff = EEPROM_Read(0x01);//��ʼ���Ƕ�
	if(( EP_FLAG&0x04) == 0x04) Dir_rand = Motor_dir(EEPROM_Read(0x02));//��ʼ������
	if(Mode)
	{
		if(( EP_FLAG&0x08) == 0x08) kp = EEPROM_ReadFloat(0x03);//��ʼ��kp
		if(( EP_FLAG&0x10) == 0x10) ki = EEPROM_ReadFloat(0x0B);//��ʼ��ki
		if(( EP_FLAG&0x20) == 0x20) kd = EEPROM_ReadFloat(0x13);//��ʼ��kd
	}else//��EEPROM PID�������Ƕȣ���ʼ��PID����
	{
		setspeed = 0;
		if(( EP_FLAG&0x08) == 0x08) kp = EEPROM_ReadFloat(0x23);//��ʼ��kp
		if(( EP_FLAG&0x10) == 0x10) ki = EEPROM_ReadFloat(0x2B);//��ʼ��ki
		if(( EP_FLAG&0x20) == 0x20) kd = EEPROM_ReadFloat(0x33);//��ʼ��kd
	}
	//��ע�Ϳ��Դ����ߵ���
	
	PID_init(kp,ki,kd,skap);//PID���Ƴ�ʼ��|����-->|����Kp|����Ki|΢��Kd|��ռ��skap
	
	delay_ms(200);//��ʱ200ms

	BEEP = 0;//�رշ�������ʾ��ʼ�����
	while(1)
	{
		
		key = GetKey();//��ȡ��ֵ
		
		switch(key)//��������
		{
			case 1:setspeed++;EEPROM_Write(0x00,setspeed);EP_FLAG |= 0x01;EEPROM_Write(0x1B,EP_FLAG);delay_ms(50);break;
			//�����趨���ٶ�|ÿ�����Ӷ���ˢ��EEPROM�Ķ�Ӧ�ڴ��ַ
			case 2:setangle_buff++;EEPROM_Write(0x01,setangle_buff);EP_FLAG |= 0x02;EEPROM_Write(0x1B,EP_FLAG);delay_ms(50);break;
			//�����趨�ĽǶ�|ÿ�����Ӷ���ˢ��EEPROM�Ķ�Ӧ�ڴ��ַ
			case 3:buff = skap;skap = 0;delay_ms(100);skap = buff;Dir_rand = Motor_dir(1);EEPROM_Write(0x02,Dir_rand);EP_FLAG |= 0x04;EEPROM_Write(0x1B,EP_FLAG);break;
			//���õ��Ϊ����|ÿ�ΰ��¶���ˢ��EEPROM�Ķ�Ӧ�ڴ��ַ
			case 4:Apply(Mode);delay_ms(50);break;
			//Ӧ�ø���|ģʽ1��Ӧ�ö� PID���� �ĸ���|ģʽ2��Ӧ�ö� �Ƕ�|PID���� �ĸ���
			case 5:setspeed--;EEPROM_Write(0x00,setspeed);EP_FLAG |= 0x01;delay_ms(50);EEPROM_Write(0x1B,EP_FLAG);break;
			//�����趨���ٶ�|ÿ�ν��Ͷ���ˢ��EEPROM�Ķ�Ӧ�ڴ��ַ
			case 6:setangle_buff--;EEPROM_Write(0x01,setangle_buff);EP_FLAG |= 0x02;delay_ms(50);EEPROM_Write(0x1B,EP_FLAG);break;
			//�����趨�ĽǶ�|ÿ�ν��Ͷ���ˢ��EEPROM�Ķ�Ӧ�ڴ��ַ
			case 7:buff = skap;skap = 0;delay_ms(100);skap = buff;Dir_rand = Motor_dir(0);EEPROM_Write(0x02,Dir_rand);EP_FLAG |= 0x04;EEPROM_Write(0x1B,EP_FLAG);break;
			//���õ��Ϊ��|ÿ�ΰ��¶���ˢ��EEPROM�Ķ�Ӧ�ڴ��ַ
			case 8:if(mp>0.00001) mp *= 0.1;else mp = 1.0;delay_ms(50);break;
			//����PID����ϵ��|ÿ�ΰ��¼�С1��
			case 9:kp += 1 * mp;if(Mode){EEPROM_WriteFloat(0x03,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x23,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//���ݵ���ϵ������KP|����ģʽ��������ڶ�Ӧ��ַ
			case 10:Mode = !Mode;Mode_Switch(Mode);delay_ms(100);break;
			//�趨 �ٶ�|�Ƕ� ģʽ�л�	
			case 11:ki += 1 * mp;if(Mode){EEPROM_WriteFloat(0x0B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x2B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//���ݵ���ϵ������KI|����ģʽ��������ڶ�Ӧ��ַ
			case 12:kd += 1 * mp;if(Mode){EEPROM_WriteFloat(0x13,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x33,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//���ݵ���ϵ������KD|����ģʽ��������ڶ�Ӧ��ַ
			case 13:kp -= 1 * mp;if(Mode){EEPROM_WriteFloat(0x03,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x23,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//���ݵ���ϵ������KP|����ģʽ��������ڶ�Ӧ��ַ
			case 14:/*����*/;break;
			//����|�˰�����
			case 15:ki -= 1 * mp;if(Mode){EEPROM_WriteFloat(0x0B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x2B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//���ݵ���ϵ������KI|����ģʽ��������ڶ�Ӧ��ַ
			case 16:kd -= 1 * mp;if(Mode){EEPROM_WriteFloat(0x13,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x33,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
			//���ݵ���ϵ������KD|����ģʽ��������ڶ�Ӧ��ַ
			default:break;
		}
		
		/*��ռ������|���Եõ���Ч���ڷ�ΧΪ999-350*/
		/*ʵ�ʻᳬ��|���������޶��������޴������*/
		if(skap>999)
		{
			skap = 999;
		}else if(skap<350)
		{
			skap = 350;
		}
		
		/*�ٶ�����|�涨��ΧΪ0-70*/
		/*֮ǰ��25-70|��Ϊ�����Ƕ�ʱ�ٶȿ��Դﵽ0���ʸ��ģ�����ʵ���ٶȷ�Χ��ȻΪ25-30*/
		if(setspeed>70)
		{
			setspeed = 70;
		}else if(setspeed<0)
		{
			setspeed = 0;
		}
		
		/*�����󱨾�*/
		if((setspeed-Motor_Read_Speed()>20||setspeed-Motor_Read_Speed()<-20) && Mode)//����20������ģʽ1�±���
		{
			BEEP = 1;
		}else 
		{
			BEEP = 0;
		}
		
		Motor_Set_Speed(setspeed);//�ٶȵ�������
		
		TIM_SetCompare1(TIM3,skap);//��ռ�ȵ�������
		
		//Select_order();//��ȡ���ڷ��͹���������
		
		UI_Working();//�û�ͼ�ν������
		
		
		if(!Mode)//ģʽ2�¸��½Ƕ�
			Motor_Set_Angle(setangle);
		
	}
}


/**
*		ģʽѡ����
*		0��ģʽ2 1��ģʽ1
**********************/
void Mode_Switch(u8 mode)
{
	if(mode)
	{
		setspeed = EEPROM_Read(0x00);
		kp = EEPROM_ReadFloat(0x03);//��ʼ��kp
		ki = EEPROM_ReadFloat(0x0B);//��ʼ��ki
		kd = EEPROM_ReadFloat(0x13);//��ʼ��kd
		PID_init(kp,ki,kd,skap);//PID���Ƴ�ʼ��|����-->|����Kp|����Ki|΢��Kd|��ռ��skap
	}else
	{
		setspeed = 0;
		kp = EEPROM_ReadFloat(0x23);//��ʼ��kp
		ki = EEPROM_ReadFloat(0x2B);//��ʼ��ki
		kd = EEPROM_ReadFloat(0x33);//��ʼ��kd
		PID_init(kp,ki,kd,skap);//PID���Ƴ�ʼ��|����-->|����Kp|����Ki|΢��Kd|��ռ��skap
	}
}


/**
*		Ӧ�ñ��
*		ģʽ2��Ӧ�� PID����|�ٶ��ϴ���ģʽ1��Ӧ��PID����
**************************************************/
void Apply(u8 mode)
{
	if(mode)
	{
		kp = EEPROM_ReadFloat(0x03);//��ʼ��kp
		ki = EEPROM_ReadFloat(0x0B);//��ʼ��ki
		kd = EEPROM_ReadFloat(0x13);//��ʼ��kd
		PID_init(kp,ki,kd,skap);//PID���Ƴ�ʼ��|����-->|����Kp|����Ki|΢��Kd|��ռ��skap
	}else
	{
		setangle = setangle_buff;
		kp = EEPROM_ReadFloat(0x23);//��ʼ��kp
		ki = EEPROM_ReadFloat(0x2B);//��ʼ��ki
		kd = EEPROM_ReadFloat(0x33);//��ʼ��kd
		PID_init(kp,ki,kd,skap);//PID���Ƴ�ʼ��|����-->|����Kp|����Ki|΢��Kd|��ռ��skap
	}
}
