#include "key.h"
#include "delay.h"
#include "sys.h" 
#include "delay.h"

/**
*		��������KEY_Init
*		���ã����󰴼�ɨ���ʼ����ʱ��ʹ�ܣ�IO�����ã�
*		��ڲ�������
*		����ֵ����
*		�޸����ڣ�2019��8��13��11:15:29
*/


void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);//ʹ��PORTFʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //���ó��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��GPIO
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //���ó���������
 	GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��GPIO

}


/**
*		��������GetKey
*		���ã����󰴼�ɨ���ȡ��ֵ
*		��ڲ�������
*		����ֵ��int�ͼ�ֵ��1-16���ް�������Ĭ�Ϸ���0
*		�޸����ڣ�2019��8��13��11:21:02
*/

/**
*		����������|��ѧ����
*		14�Ű����޷���ȡ ���ϵ�71�д������Ե���ԭ��δ֪|ʧ��|���������ã�������14�Ű���
**/
int GetKey(void)//������ʼ������
{
	int y;//���浱ǰɨ�����
	char state = 0x00;//���水��״̬
	int buff;//��ʱ����
	
	PFout(0) = 1;
	PFout(1) = 1;
	PFout(2) = 1;
	PFout(3) = 1;
	
	if(PFin(4)|PFin(5)|PFin(6)|PFin(7))//�������
	{
		delay_ms(5);
		if(PFin(4)|PFin(5)|PFin(6)|PFin(7))//���ȷʵ����
		{
			for(y=0;y<4;y++)//��ɨ��
			{
				switch(y)
				{
					case 0:PFout(0) = 1;PFout(1) = 0;PFout(2) = 0;PFout(3) = 0;break;
					case 1:PFout(0) = 0;PFout(1) = 1;PFout(2) = 0;PFout(3) = 0;break;
					case 2:PFout(0) = 0;PFout(1) = 0;PFout(2) = 1;PFout(3) = 0;break;
					case 3:PFout(0) = 0;PFout(1) = 0;PFout(2) = 0;PFout(3) = 1;break;
				}
				
				for(buff=0;buff<4;buff++)//��ȡ��ǰ��F4-F7��IO�ڵ�ƽ ����state�ĵ�4λ��
				{
					if(PFin( (buff+4) ))
					{
						state |= 1<<(buff);
					}else{
						state |= 0<<(buff);
					}
				}
				if(state!=0x00)//�ڵ�ǰ���¼�⵽����
				{
					switch(state)//���ؼ�ֵ
					{
						case 0x01:return 4 * y + 1;
						case 0x02:return 4 * y + 2;
						case 0x04:return 4 * y + 3;
						case 0x08:return 4 * y + 4;
						//default:return 14;//���ϴ��п����޸�14�Ű����޷���ȡ��BUG ԭ��δ֪|������
					}
				}
			}
		}
	}
	return 0;//�ް������·���0
}
