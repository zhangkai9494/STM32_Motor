#include "menu.h"
#include "sys.h"
#include "usart.h"
#include "motor.h"
#include "pid.h"
#include "EEPROM.h"
#include "printf.h" 

/*�ⲿ��������*/
extern void Mode_Switch(u8 mode);


/**
*		�˵�ѡ����
*		���մ��ڷ��͵�ָ�������Ӧ����
*   �����б�
*		help 																								//����������˵�
*		setspeed [1\2\3λint������]													//����ָ��ת������
*		setangle [1\2\3λint������]													//����ָ���Ƕ�
*		setdir [0\1] 																				//�趨���з��� 0��ת 1����
*		setkp [һλ��������5λС�����ֵ�float������]					//�趨PID���� ����ϵ��
*		setki [һλ��������5λС�����ֵ�float������]					//�趨PID���� ����ϵ��
*		setkd [һλ��������5λС�����ֵ�float������]					//�趨PID���� ΢��ϵ��
*		setmode [0\1]																				//�趨����ģʽ 0�Ƕ� 1�ٶ�
*
*		2019��8��12��12:37:26�¸��£�������� -d -h -u ָ����������λ��
*   2019��8��14��17:04:02�¸��£�����ָ��setmode �޸�setkp/i/d����Ӳ�ͬģʽ��EEPROM��������ѡ��Ĺ���
*		2019��8��18��12:20:28�޸���setangle -h xxָ��
*		2019��8��23��19:49:35�޸ģ�����������ʹ���ص�help�б����������
*/

/*�ڲ�������Char������ת��Ϊint��*/
int Char_to_int(char command)
{
	int number;
	number = (int)command - 48;
	if(number<0)number = 0;
	return number;
}

void Select_order(void)
{
	int Data_length;
	int buff;
	char Command[25];
	
	/*�ⲿ��������|�����޸�*/
	extern u8 Mode;	//���Բ��ã����ǰ����ʹ���֮�佫�޷����� ������Ȼ����ȫ���޸ķ���
	extern int setspeed;
	extern int setangle;
	extern int setangle_buff;
	extern u8 Dir_rand;
	extern float kp,ki,kd;
	extern int skap;
	
	if(USART_RX_STA&0x8000)//������յ����� ��ʼ����ָ��
	{
		Data_length = USART_RX_STA&0x3fff;//��ȡ��ȡ�������ݳ���

		//�յ������ݷ���ָ�����|û�����������
		for(buff=0;buff<Data_length;buff++)
		{
			Command[buff] = USART_RX_BUF[buff];
		}
	
		//����Ա�/ѡ��
/**************************************************************************************************/// ����ָ��
		if(Command[0] == 'h'&& Command[1] == 'e' && Command[2] == 'l' && Command[3] == 'p')//ָ����ʾ��
		{
			Printf("Command:\r\n");
			Printf("  setspeed     [1|2|3 bit int]\r\n");
			Printf("  setangle     [1|2|3 bit int]\r\n");
			Printf("  setmode      [0|1]\r\n");
			Printf("  setdir       [0|1]\r\n");
			Printf("  setkp        [float]\r\n");
			Printf("  setki        [float]\r\n");
			Printf("  setkd        [float]\r\n");
			Printf("\r\n");
			Printf("Parameter:\r\n");
			Printf("  Use '-h' to contral hundred bit\r\n");
			Printf("  Use '-d' to contral ten bit\r\n");
			Printf("  Use '-u' to contral bits bit\r\n");
			Printf("\r\n");
			Printf("Example: setspeed -d 40\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///�趨�ٶ�ָ��
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 's' && Command[4] == 'p' && Command[5] == 'e'&& Command[6] == 'e' && Command[7] == 'd')
		{
			if(Char_to_int(Command[9]) != 0 && Char_to_int(Command[10]) != 0 && Char_to_int(Command[11])!= 0)//�����һ����λ��������λ��������0��
			{
				setspeed = Char_to_int(Command[9]) * 100 + Char_to_int(Command[10] * 10) + Char_to_int(Command[11]);
			}else if(Char_to_int(Command[9]) != 0 && Char_to_int(Command[10]) != 0 && Command[11] == '\0')//�����һ����λ��������λ��������0���ҵ�����λΪ0��
			{
				setspeed = Char_to_int(Command[9]) * 10 + Char_to_int(Command[10]);
			}else if(Char_to_int(Command[9]) != 0 && Command[10] == '\0')//�����һλ����������ֲ���Ϊ�㲢�ҵڶ���λΪ0��
			{
				setspeed = Char_to_int(Command[9]);
			}else if(Command[9] == '-')//��������˲���
			{
				if(Command[10] == 'u')
				{
					setspeed = Char_to_int(Command[12]);
				}else if(Command[10] == 'd')
				{
					setspeed = Char_to_int(Command[12]) * 10 + Char_to_int(Command[13]);
				}else if(Command[10] == 'h')
				{
					setspeed = Char_to_int(Command[12]) * 100 + Char_to_int(Command[13]) * 10+ Char_to_int(Command[14]);
				}else
				{
					Printf("Wrong Input!\r\n");//����Ƿ���ʾ
				}
			}else
			{
				Printf("Wrong!\r\n");//����Ƿ���ʾ
			}
			EEPROM_Write(0x00,setspeed);//����EEPROM
			Printf("Order:[setspeed] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///�趨�Ƕ�ָ��
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'a' && Command[4] == 'n' && Command[5] == 'g'&& Command[6] == 'l' && Command[7] == 'e')
		{
			if(Char_to_int(Command[9]) != 0 && Char_to_int(Command[10]) != 0 && Char_to_int(Command[11])!= 0)
			{
				setangle_buff = Char_to_int(Command[9]) * 100 + Char_to_int(Command[10] * 10) + Char_to_int(Command[11]);
				printf("%d",setangle_buff);
			}else if(Char_to_int(Command[9]) != 0 && Char_to_int(Command[10]) != 0 && Command[11] == '\0')
			{
				setangle_buff = Char_to_int(Command[9]) * 10 + Char_to_int(Command[10]);
			}else if(Char_to_int(Command[9]) != 0 && Command[10] == '\0')
			{
				setangle_buff = Char_to_int(Command[9]);
			}else if(Command[9] == '-')//��������˲���
			{
				if(Command[10] == 'u')
				{
					setangle_buff = Char_to_int(Command[12]);
				}else if(Command[10] == 'd')
				{
					setangle_buff = Char_to_int(Command[12]) * 10 + Char_to_int(Command[13]);
				}else if(Command[10] == 'h')
				{
					setangle_buff = Char_to_int(Command[12]) * 100 + Char_to_int(Command[13]) * 10 + Char_to_int(Command[14]);
				}else
				{
					Printf("Wrong Input!\r\n");//����Ƿ���ʾ
				}
			}else
			{
				Printf("Wrong!\r\n");//����Ƿ���ʾ
			}
			EEPROM_Write(0x01,setangle_buff);//����EEPROM
			setangle = setangle_buff;//���ĺ�����ִ��
			Printf("Order:[setangle] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///�趨ģʽָ��
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'm' && Command[4] == 'o' && Command[5] == 'd' && Command[6] == 'e')
		{
			if(Char_to_int(Command[8]) == 0)
			{
				Mode = 0;
				Mode_Switch(Mode);
			}else if(Char_to_int(Command[8]) == 1)
			{
				Mode = 1;
				Mode_Switch(Mode);
			}else
			{
				Printf("Wrong Input!\r\n");//����Ƿ���ʾ
			}
			Printf("Order:[setmode] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///�趨ת��ָ��
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
				Printf("Wrong Input!\r\n");//����Ƿ���ʾ
			}
			Printf("Order:[setdir] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///�趨PID���� P
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'k' && Command[4] == 'p')
		{
			if(Command[7] == '.' && Command[13]== '\0')
			{
				kp = Char_to_int(Command[6]) + (float)Char_to_int(Command[8]) * 0.1 + (float)Char_to_int(Command[9]) * 0.01 + (float)Char_to_int(Command[10]) * 0.001 + (float)Char_to_int(Command[11]) * 0.0001 + (float)Char_to_int(Command[12]) * 0.00001;
				if(Mode)
					EEPROM_WriteFloat(0x03,kp);
				else
					EEPROM_WriteFloat(0x23,kp);
				PID_init(kp,ki,kd,skap);
			}else
			{
				Printf("Wrong Input!\r\n");//����Ƿ���ʾ
			}
			Printf("Order:[setkp] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///�趨PID���� I
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'k' && Command[4] == 'i')
		{
			if(Command[7] == '.' && Command[13]== '\0')
			{
				ki = Char_to_int(Command[6]) + (float)Char_to_int(Command[8]) * 0.1 + (float)Char_to_int(Command[9]) * 0.01 + (float)Char_to_int(Command[10]) * 0.001 + (float)Char_to_int(Command[11]) * 0.0001 + (float)Char_to_int(Command[12]) * 0.00001;
				if(Mode)
					EEPROM_WriteFloat(0x0B,ki);
				else
					EEPROM_WriteFloat(0x2B,ki);
				PID_init(kp,ki,kd,skap);
			}else
			{
				Printf("Wrong Input!\r\n");//����Ƿ���ʾ
			}
			Printf("Order:[setki] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///�趨PID���� D
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'k' && Command[4] == 'd')
		{
			if(Command[7] == '.' && Command[13]== '\0')
			{
				kd = Char_to_int(Command[6]) + (float)Char_to_int(Command[8]) * 0.1 + (float)Char_to_int(Command[9]) * 0.01 + (float)Char_to_int(Command[10]) * 0.001 + (float)Char_to_int(Command[11]) * 0.0001 + (float)Char_to_int(Command[12]) * 0.00001;
				if(Mode)
					EEPROM_WriteFloat(0x13,kd);
				else
					EEPROM_WriteFloat(0x33,kd);
				PID_init(kp,ki,kd,skap);
			}else
			{
				Printf("Wrong Input!\r\n");//����Ƿ���ʾ
			}
			Printf("Order:[setkd] Excute successfully!\r\n");
			Printf("======================================\r\n");
		}else
		{
			Printf("Wrong Order!\r\n");
			Printf("======================================\r\n");
		}
/**************************************************************************************************/
		USART_RX_STA=0;
	}
}
