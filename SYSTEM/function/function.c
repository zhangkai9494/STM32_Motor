#include "function.h"
#include "EEPROM.h"
#include "motor.h"
#include "delay.h"
#include "pid.h"
#include "beep.h"

extern int setspeed;
extern char EP_FLAG;
extern int setangle_buff;
extern int skap;
extern u8 Dir_rand;
extern u8 Mode;
extern float mp;
extern float kp;
extern float ki;
extern float kd;
extern int setangle;


/**
*		��������Apply
*		���ã���ͬģʽ�� Ӧ�ö�PID�������Ƕ��趨�ı��
*		��ڲ�����uchar��ģʽ��־
*		����ֵ����
*		�޸����ڣ�2019��8��11��14:13:37
*/

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


/**
*		��������Mode_Switch
*		���ã����趨�ٶȺ��趨�Ƕ�ģʽ�н���ת��
*		��ڲ�����uchar��ģʽ��־
*		����ֵ����
*		�޸����ڣ�2019��8��11��14:11:25
*/

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


void Key_function(u8 key)
{
	int buff;
	
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
		//���õ����ת|ÿ�ΰ��¶���ˢ��EEPROM�Ķ�Ӧ�ڴ��ַ
		case 8:if(mp>0.00001) mp *= 0.1;else mp = 1.0;delay_ms(50);break;
		//����PID����ϵ��|ÿ�ΰ��¼�С10��
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
}

void SpeedWarning_function(void)//ʧ�ٱ�������
{
	if((setspeed-Motor_Read_Speed()>20||setspeed-Motor_Read_Speed()<-20) && Mode)//����20������ģʽ1�±���
	{
		BEEP = 1;
	}else 
	{
		BEEP = 0;
	}
}

void SpeedLimi_function(void)//�ٶ����ƺ���
{
	/*֮ǰ��25-70|��Ϊ�����Ƕ�ʱ�ٶȿ��Դﵽ0���ʸ��ģ�����ʵ���ٶȷ�Χ��ȻΪ25-30*/
	if(setspeed>70)
	{
		setspeed = 70;
	}else if(setspeed<0)
	{
		setspeed = 0;
	}
}

void SkipLimit_function(void)//��ռ�����ƺ���
{
	if(skap>999)
	{
		skap = 999;
	}else if(skap<350)
	{
		skap = 350;
	}
}

void InitializeByEEPROM_function(void)//��EEPROM��ʼ������
{
	EP_FLAG = EEPROM_Read(0x1B);//��ȡ��־λ,���ݱ�־λ��ʼ��
	if(( EP_FLAG&0x01) == 0x01) setspeed = EEPROM_Read(0x00);//��ʼ���ٶ�
	if(( EP_FLAG&0x02) == 0x02) setangle_buff = EEPROM_Read(0x01);//��ʼ���Ƕ�
	if(( EP_FLAG&0x04) == 0x04) Dir_rand = Motor_dir(EEPROM_Read(0x02));//��ʼ������
	if(Mode)//��EEPROM PID�������ٶȣ���ʼ��PID����
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
}
