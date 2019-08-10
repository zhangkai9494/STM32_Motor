#include "printf.h"
#include <stdarg.h>
#include "usart.h"
#include "sys.h"

/**
*		Printf函数 功能同printf函数 可以识别 |\n \r| 两种格式控制符 |%s %d| 两种数据类型控制符
*		引用标准库中<stdarg.h>头文件 用于确定不定参数的个数
*		va_list ap;//创建参数地址表列
*		va_start(ap, Data);//对ap 进行初始化，让它指向可变参数表里面的第一个参数，
*/

void Printf(char* Data, ... )
{
	const char *s;
	int d;  
	float f;	
	char buf[16];//储存int型数据转化为char型
	
	va_list ap;//创建参数地址表列
	va_start(ap, Data);//对ap 进行初始化，让它指向可变参数表里面的第一个参数Data，

	while (*Data != 0 )// 判断是否到达字符串结束符
	{				                          
		if (*Data == 0x5c )//判断是否为反斜杠
		{									  
			switch (*++Data )//根据反斜杠后的标识符执行格式控制
			{
				case 'r':
				USART_SendData(USART1, 0x0d);//发送一个回车
				Data ++;
				break;

				case 'n':
				USART_SendData(USART1, 0x0a);//进行换行
				Data ++;
				break;

				default://非标准格式控制符
				Data ++;
				break;
			}			 
		}
		else if (*Data == '%')//如果是类型空字符
		{
			switch (*++Data)
			{				
				case 's': //字符串
				s = va_arg(ap, const char *);
				
				for ( ; *s; s++) 
				{
					USART_SendData(USART1,*s);
					while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
				}
				Data++;
				break;

				case 'd'://十进制
				d = va_arg(ap, int);
				
				sprintf(buf,"%d",d);//转换为字符串
				
				for (s = buf; *s; s++) 
				{
					USART_SendData(USART1,*s);
					while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
				}
				Data++;
				break;
				
				case 'f':
				f = va_arg(ap, double);
				sprintf(buf,"%f",f);//转换为字符串
				
				for (s = buf; *s; s++) 
				{
					USART_SendData(USART1,*s);
					while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
				}
				
				default://不是有效数据类型控制符
				Data++;
				break;
				
			}
		}
		
		else USART_SendData(USART1, *Data++);
		
		while ( USART_GetFlagStatus ( USART1, USART_FLAG_TXE ) == RESET );
		
	}
}
