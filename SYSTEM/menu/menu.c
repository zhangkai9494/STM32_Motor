#include "menu.h"
#include "sys.h"
#include "usart.h"
#include "motor.h"
#include "pid.h"
#include "EEPROM.h"
#include "printf.h" 

/*外部函数调用*/
extern void Mode_Switch(u8 mode);


/**
*		菜单选择函数
*		接收串口发送的指令并作出相应处理
*   命令列表：
*		help 																								//打开命令帮助菜单
*		setspeed [1\2\3位int型数据]													//按照指定转速运行
*		setangle [1\2\3位int型数据]													//运行指定角度
*		setdir [0\1] 																				//设定运行方向 0反转 1正传
*		setkp [一位整数部分5位小数部分的float型数据]					//设定PID参数 比例系数
*		setki [一位整数部分5位小数部分的float型数据]					//设定PID参数 积分系数
*		setkd [一位整数部分5位小数部分的float型数据]					//设定PID参数 微分系数
*		setmode [0\1]																				//设定运行模式 0角度 1速度
*
*		2019年8月12日12:37:26新更新：加入参数 -d -h -u 指定输入数字位数
*   2019年8月14日17:04:02新更新：加入指令setmode 修改setkp/i/d，添加不同模式下EEPROM储存区块选择的功能
*		2019年8月18日12:20:28修复：setangle -h xx指令
*		2019年8月23日19:49:35修改：调整缩进，使返回的help列表更加美观了
*/

/*内部函数将Char型数据转换为int型*/
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
	
	/*外部变量引用|方便修改*/
	extern u8 Mode;	//可以不用，但是按键和串口之间将无法联动 所以仍然保留全局修改方法
	extern int setspeed;
	extern int setangle;
	extern int setangle_buff;
	extern u8 Dir_rand;
	extern float kp,ki,kd;
	extern int skap;
	
	if(USART_RX_STA&0x8000)//如果接收到数据 开始分析指令
	{
		Data_length = USART_RX_STA&0x3fff;//获取获取到的数据长度

		//收到的数据放入指令缓存区|没有做溢出处理
		for(buff=0;buff<Data_length;buff++)
		{
			Command[buff] = USART_RX_BUF[buff];
		}
	
		//命令对比/选择
/**************************************************************************************************/// 帮助指令
		if(Command[0] == 'h'&& Command[1] == 'e' && Command[2] == 'l' && Command[3] == 'p')//指令提示符
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
/**************************************************************************************************///设定速度指令
		}else if(Command[0] == 's'&& Command[1] == 'e' && Command[2] == 't' && Command[3] == 's' && Command[4] == 'p' && Command[5] == 'e'&& Command[6] == 'e' && Command[7] == 'd')
		{
			if(Char_to_int(Command[9]) != 0 && Char_to_int(Command[10]) != 0 && Char_to_int(Command[11])!= 0)//如果是一个三位数（三个位都不能是0）
			{
				setspeed = Char_to_int(Command[9]) * 100 + Char_to_int(Command[10] * 10) + Char_to_int(Command[11]);
			}else if(Char_to_int(Command[9]) != 0 && Char_to_int(Command[10]) != 0 && Command[11] == '\0')//如果是一个两位数（两个位都不能是0并且第三个位为0）
			{
				setspeed = Char_to_int(Command[9]) * 10 + Char_to_int(Command[10]);
			}else if(Char_to_int(Command[9]) != 0 && Command[10] == '\0')//如果是一位数（这个数字不能为零并且第二个位为0）
			{
				setspeed = Char_to_int(Command[9]);
			}else if(Command[9] == '-')//如果输入了参数
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
					Printf("Wrong Input!\r\n");//输入非法提示
				}
			}else
			{
				Printf("Wrong!\r\n");//输入非法提示
			}
			EEPROM_Write(0x00,setspeed);//更改EEPROM
			Printf("Order:[setspeed] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///设定角度指令
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
			}else if(Command[9] == '-')//如果输入了参数
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
					Printf("Wrong Input!\r\n");//输入非法提示
				}
			}else
			{
				Printf("Wrong!\r\n");//输入非法提示
			}
			EEPROM_Write(0x01,setangle_buff);//更改EEPROM
			setangle = setangle_buff;//更改后立即执行
			Printf("Order:[setangle] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///设定模式指令
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
				Printf("Wrong Input!\r\n");//输入非法提示
			}
			Printf("Order:[setmode] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///设定转向指令
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
				Printf("Wrong Input!\r\n");//输入非法提示
			}
			Printf("Order:[setdir] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///设定PID参数 P
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
				Printf("Wrong Input!\r\n");//输入非法提示
			}
			Printf("Order:[setkp] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///设定PID参数 I
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
				Printf("Wrong Input!\r\n");//输入非法提示
			}
			Printf("Order:[setki] Excute successfully!\r\n");
			Printf("======================================\r\n");
/**************************************************************************************************///设定PID参数 D
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
				Printf("Wrong Input!\r\n");//输入非法提示
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
