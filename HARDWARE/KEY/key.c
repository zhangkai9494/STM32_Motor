#include "key.h"
#include "delay.h"
#include "sys.h" 
#include "delay.h"

/**
*		函数名：KEY_Init
*		作用：矩阵按键扫描初始化（时钟使能，IO口配置）
*		入口参数：无
*		返回值：无
*		修改日期：2019年8月13日11:15:29
*/


void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);//使能PORTF时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置成推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIO
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;//KEY0-KEY1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //设置成下拉输入
 	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIO

}


/**
*		函数名：GetKey
*		作用：矩阵按键扫描获取键值
*		入口参数：无
*		返回值：int型键值（1-16）无按键按下默认返回0
*		修改日期：2019年8月13日11:21:02
*/

/**
*		按键有问题|玄学问题
*		14号按键无法读取 加上第71行代码后可以但是原因未知|失败|方法不管用，放弃第14号按键
**/
int GetKey(void)//按键初始化函数
{
	int y;//储存当前扫描的行
	char state = 0x00;//储存按键状态
	int buff;//临时变量
	
	PFout(0) = 1;
	PFout(1) = 1;
	PFout(2) = 1;
	PFout(3) = 1;
	
	if(PFin(4)|PFin(5)|PFin(6)|PFin(7))//如果按下
	{
		delay_ms(5);
		if(PFin(4)|PFin(5)|PFin(6)|PFin(7))//如果确实按下
		{
			for(y=0;y<4;y++)//行扫描
			{
				switch(y)
				{
					case 0:PFout(0) = 1;PFout(1) = 0;PFout(2) = 0;PFout(3) = 0;break;
					case 1:PFout(0) = 0;PFout(1) = 1;PFout(2) = 0;PFout(3) = 0;break;
					case 2:PFout(0) = 0;PFout(1) = 0;PFout(2) = 1;PFout(3) = 0;break;
					case 3:PFout(0) = 0;PFout(1) = 0;PFout(2) = 0;PFout(3) = 1;break;
				}
				
				for(buff=0;buff<4;buff++)//获取当前的F4-F7的IO口电平 存在state的低4位中
				{
					if(PFin( (buff+4) ))
					{
						state |= 1<<(buff);
					}else{
						state |= 0<<(buff);
					}
				}
				if(state!=0x00)//在当前行下检测到按下
				{
					switch(state)//返回键值
					{
						case 0x01:return 4 * y + 1;
						case 0x02:return 4 * y + 2;
						case 0x04:return 4 * y + 3;
						case 0x08:return 4 * y + 4;
						//default:return 14;//加上此行可以修复14号按键无法读取的BUG 原因未知|不管用
					}
				}
			}
		}
	}
	return 0;//无按键按下返回0
}
