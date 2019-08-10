#include "menu.h"
#include "sys.h"
#include "usart.h"
#include "motor.h"
#include "pid.h"
//#include "EEPROM.h"

#include "printf.h" 

/**
*		菜单选择函数
*		接收串口发送的指令并作出相应处理
*   命令列表：
*		help 																								//打开命令帮助菜单
*		setspeed [1\2\3位int型数据]													//按照指定转速运行
*		setangle [1\2\3位int型数据]													//运行指定角度
*		setdir [0\1] 																				//设定运行方向 0反转 1正传
*		setkp [一位整数部分两位小数部分的float型数据]					//设定PID参数 比例系数
*		setki [一位整数部分两位小数部分的float型数据]					//设定PID参数 积分系数
*		setkd [一位整数部分两位小数部分的float型数据]					//设定PID参数 微分系数
*/

/*内部函数将Char型数据转换为int型*/
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
	
	/*外部变量引用|方便修改*/
	extern int setspeed;
	extern int setangle;
	extern u8 Dir_rand;
	extern float kp,ki,kd;
	extern int skap;
		Data_length = USART_RX_STA&0x3fff;

		//收到的数据放入缓存区
		for(buff=0;buff<Data_length;buff++)
		{
			Command[buff] = USART_RX_BUF[buff];
		}
	
		//命令对比/选择
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
				;//输入非法提示
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
				;//输入非法提示
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
				;//输入非法提示
			}
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'k' && Command[4] == 'p')
		{
			if(Command[7] == '.' && Command[10]== '\0')
			{
				kp = Char_to_int(Command[6]) + (float)Char_to_int(Command[8]) * 0.1 + (float)Char_to_int(Command[9]) * 0.01;
				PID_init(kp,ki,kd,skap);
			}else
			{
				;//输入非法提示
			}
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'k' && Command[4] == 'i')
		{
			if(Command[7] == '.' && Command[10]== '\0')
			{
				ki = Char_to_int(Command[6]) + (float)Char_to_int(Command[8]) * 0.1 + (float)Char_to_int(Command[9]) * 0.01;
				PID_init(kp,ki,kd,skap);
			}else
			{
				;//输入非法提示
			}
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 'k' && Command[4] == 'd')
		{
			if(Command[7] == '.' && Command[10]== '\0')
			{
				kd = Char_to_int(Command[6]) + (float)Char_to_int(Command[8]) * 0.1 + (float)Char_to_int(Command[9]) * 0.01;
				PID_init(kp,ki,kd,skap);
			}else
			{
				;//输入非法提示
			}
		}
	}