#include "EEPROM.h"
#include "myiic.h"
#include "delay.h"

void EEPROM_Init(void)//EEPROM初始化|IIC总线初始化
{
	IIC_Init();
}


u8 EEPROM_Read(u16 addr)//读取指定地址上的一个字节的数据值
{
	u8 buff=0;
	IIC_Start();
	IIC_Send_Byte(0XA0+((addr/256)<<1)); //发送器件地址 0XA0,写数据
	IIC_Wait_Ack();
	IIC_Send_Byte(addr%256); //发送低地址
	IIC_Wait_Ack();
	IIC_Start();
	IIC_Send_Byte(0XA1); //进入接收模式
	IIC_Wait_Ack();
	buff=IIC_Read_Byte(0);
	IIC_Stop(); //产生一个停止条件
	return buff;
}


void EEPROM_Write(u16 addr,u8 SentData)//向指定地址上写一个字节数据
{
	IIC_Start();
	IIC_Send_Byte(0XA0+((addr/256)<<1)); //发送器件地址 0XA0,写数据
	IIC_Wait_Ack();
	IIC_Send_Byte(addr%256); //发送低地址
	IIC_Wait_Ack();
	IIC_Send_Byte(SentData); //发送字节
	IIC_Wait_Ack();
	IIC_Stop(); //产生一个停止条件
	delay_ms(10);
}



void EEPROM_WriteFloat(u16 addr,float SentData)//向指定地址上写一个8字节的float型数据
{
	unsigned char buff[8];
	int t;
	/*现将float转换为char型数组*/
	buff[0] = (int)SentData%10+48;
	buff[1] = '.';
	buff[2] = (int)(SentData*10)%10+48;
	buff[3] = (int)(SentData*100)%10+48;
	buff[4] = (int)(SentData*1000)%10+48;
	buff[5] = (int)(SentData*10000)%10+48;
	buff[6] = (int)(SentData*100000)%10+48;
	buff[7] = (int)(SentData*1000000)%10+48;
	/*开始写入*/
	for(t=0;t<8;t++)
	{
		EEPROM_Write(addr+t,buff[t]);
	}
}


float EEPROM_ReadFloat(u16 addr)//读取指定地址上的8字节float型数据
{ 
	unsigned char buff[8];
	float result;
	int i;
	for(i=0;i<8;i++)
	{
		buff[i] = EEPROM_Read(addr+i)-48;
	}
	
	//结果转换
	result = buff[0]+buff[2]*0.1+buff[3]*0.01+buff[4]*0.001+buff[5]*0.0001+buff[6]*0.00001+buff[7]*0.000001;
	
	return result;
}
