#include "printf.h"
#include <stdarg.h>
#include "usart.h"
#include "sys.h"

/**
*		Printf���� ����ͬprintf���� ����ʶ�� |\n \r| ���ָ�ʽ���Ʒ� |%s %d| �����������Ϳ��Ʒ�
*		���ñ�׼����<stdarg.h>ͷ�ļ� ����ȷ�����������ĸ���
*		va_list ap;//����������ַ����
*		va_start(ap, Data);//��ap ���г�ʼ��������ָ��ɱ����������ĵ�һ��������
*/

void Printf(char* Data, ... )
{
	const char *s;
	int d;  
	float f;	
	char buf[16];//����int������ת��Ϊchar��
	
	va_list ap;//����������ַ����
	va_start(ap, Data);//��ap ���г�ʼ��������ָ��ɱ����������ĵ�һ������Data��

	while (*Data != 0 )// �ж��Ƿ񵽴��ַ���������
	{				                          
		if (*Data == 0x5c )//�ж��Ƿ�Ϊ��б��
		{									  
			switch (*++Data )//���ݷ�б�ܺ�ı�ʶ��ִ�и�ʽ����
			{
				case 'r':
				USART_SendData(USART1, 0x0d);//����һ���س�
				Data ++;
				break;

				case 'n':
				USART_SendData(USART1, 0x0a);//���л���
				Data ++;
				break;

				default://�Ǳ�׼��ʽ���Ʒ�
				Data ++;
				break;
			}			 
		}
		else if (*Data == '%')//��������Ϳ��ַ�
		{
			switch (*++Data)
			{				
				case 's': //�ַ���
				s = va_arg(ap, const char *);
				
				for ( ; *s; s++) 
				{
					USART_SendData(USART1,*s);
					while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
				}
				Data++;
				break;

				case 'd'://ʮ����
				d = va_arg(ap, int);
				
				sprintf(buf,"%d",d);//ת��Ϊ�ַ���
				
				for (s = buf; *s; s++) 
				{
					USART_SendData(USART1,*s);
					while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
				}
				Data++;
				break;
				
				case 'f':
				f = va_arg(ap, double);
				sprintf(buf,"%f",f);//ת��Ϊ�ַ���
				
				for (s = buf; *s; s++) 
				{
					USART_SendData(USART1,*s);
					while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
				}
				
				default://������Ч�������Ϳ��Ʒ�
				Data++;
				break;
				
			}
		}
		
		else USART_SendData(USART1, *Data++);
		
		while ( USART_GetFlagStatus ( USART1, USART_FLAG_TXE ) == RESET );
		
	}
}
