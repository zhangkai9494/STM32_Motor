#include "menu.h"
#include "sys.h"
#include "usart.h"
#include "motor.h"
#include "pid.h"
//#include "EEPROM.h"

#include "printf.h" 

/**
*		�˵�ѡ����
*		���մ��ڷ��͵�ָ�������Ӧ����
*   �����б�
*		help 																								//����������˵�
*		setspeed [1\2\3λint������]													//����ָ��ת������
*		setangle [1\2\3λint������]													//����ָ���Ƕ�
*		setdir [0\1] 																				//�趨���з��� 0��ת 1����
*		setkp [һλ����������λС�����ֵ�float������]					//�趨PID���� ����ϵ��
*		setki [һλ����������λС�����ֵ�float������]					//�趨PID���� ����ϵ��
*		setkd [һλ����������λС�����ֵ�float������]					//�趨PID���� ΢��ϵ��
*/

/*�ڲ�������Char������ת��Ϊint��*/
int Char_to_int(char command)
{
	int number;
	number = (int)command - 47;
	return number;
}

void Select_order(void)
{
	int Data_length;
	int buff;
	char Command[25];
	
	/*�ⲿ��������|�����޸�*/
	extern int setspeed;
	extern int setangle;
	extern u8 Dir_rand;
	extern float kp,ki,kd;
	extern int skap;
		Data_length = USART_RX_STA&0x3fff;

		//�յ������ݷ��뻺����
		for(buff=0;buff<Data_length;buff++)
		{
			Command[buff] = USART_RX_BUF[buff];
		}
	
		//����Ա�/ѡ��
		if(Command[0] == 'h'&& Command[1] == 'e' && Command[2] == 'l' && Command[3] == 'p')
		{
			Printf("Command:\r\n");
			Printf("setspeed [1\2\3 bit int]\r\n");
			Printf("setangle [1\2\3 bit int]:\r\n");
			Printf("setdir [0\1]\r\n");
			Printf("setkp [float]\r\n");
			Printf("setki [float]\r\n");
			Printf("setkd [float]\r\n");
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 's' && Command[4] == 'p' && Command[5] == 'e'&& Command[6] == 'e' && Command[7] == 'd')
		{
			if(Char_to_int(Command[9]) != 0 && Char_to_int(Command[10]) != 0 && Char_to_int(Command[11])!= 0)
			{
				setspeed = Char_to_int(Command[9]) * 100 + Char_to_int(Command[10] * 10) + Char_to_int(Command[11]);
			}else if(Char_to_int(Command[9]) != 0 && Char_to_int(Command[10]) != 0 && Command[11] == '\0')
			{
				setspeed = Char_to_int(Command[9]) * 10 + Char_to_int(Command[10]);
			}else if(Char_to_int(Command[9]) != 0 && Command[10] == '\0')
			{
				setspeed = Char_to_int(Command[9]);
			}else
			{
				;//����Ƿ���ʾ
			}
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'a' && Command[4] == 'n' && Command[5] == 'g'&& Command[6] == 'l' && Command[7] == 'e')
		{
			if(Char_to_int(Command[9]) != 0 && Char_to_int(Command[10]) != 0 && Char_to_int(Command[11])!= 0)
			{
				setangle = Char_to_int(Command[9]) * 100 + Char_to_int(Command[10] * 10) + Char_to_int(Command[11]);
			}else if(Char_to_int(Command[9]) != 0 && Char_to_int(Command[10]) != 0 && Command[11] == '\0')
			{
				setangle = Char_to_int(Command[9]) * 10 + Char_to_int(Command[10]);
			}else if(Char_to_int(Command[9]) != 0 && Command[10] == '\0')
			{
				setangle = Char_to_int(Command[9]);
			}else
			{
				;//����Ƿ���ʾ
			}
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'd' && Command[4] == 'i' && Command[5] == 'r')
		{
			if(Char_to_int(Command[7]) == 0)
			{
				Dir_rand = Motor_dir(0);
			}else if(Char_to_int(Command[7]) == 1)
			{
				Dir_rand = Motor_dir(1);
			}else
			{
				;//����Ƿ���ʾ
			}
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'k' && Command[4] == 'p')
		{
			if(Command[7] == '.' && Command[10]== '\0')
			{
				kp = Char_to_int(Command[6]) + (float)Char_to_int(Command[8]) * 0.1 + (float)Char_to_int(Command[9]) * 0.01;
				PID_init(kp,ki,kd,skap);
			}else
			{
				;//����Ƿ���ʾ
			}
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'k' && Command[4] == 'i')
		{
			if(Command[7] == '.' && Command[10]== '\0')
			{
				ki = Char_to_int(Command[6]) + (float)Char_to_int(Command[8]) * 0.1 + (float)Char_to_int(Command[9]) * 0.01;
				PID_init(kp,ki,kd,skap);
			}else
			{
				;//����Ƿ���ʾ
			}
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'k' && Command[4] == 'd')
		{
			if(Command[7] == '.' && Command[10]== '\0')
			{
				kd = Char_to_int(Command[6]) + (float)Char_to_int(Command[8]) * 0.1 + (float)Char_to_int(Command[9]) * 0.01;
				PID_init(kp,ki,kd,skap);
			}else
			{
				;//����Ƿ���ʾ
			}
		}
	}