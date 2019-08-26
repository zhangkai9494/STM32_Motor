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
*		函数名：Apply
*		作用：不同模式下 应用对PID参数、角度设定的变更
*		入口参数：uchar型模式标志
*		返回值：无
*		修改日期：2019年8月11日14:13:37
*/

/**
*		应用变更
*		模式2下应用 PID参数|速度上传；模式1下应用PID参数
**************************************************/
void Apply(u8 mode)
{
	if(mode)
	{
		kp = EEPROM_ReadFloat(0x03);//初始化kp
		ki = EEPROM_ReadFloat(0x0B);//初始化ki
		kd = EEPROM_ReadFloat(0x13);//初始化kd
		PID_init(kp,ki,kd,skap);//PID控制初始化|参数-->|比例Kp|积分Ki|微分Kd|空占比skap
	}else
	{
		setangle = setangle_buff;
		kp = EEPROM_ReadFloat(0x23);//初始化kp
		ki = EEPROM_ReadFloat(0x2B);//初始化ki
		kd = EEPROM_ReadFloat(0x33);//初始化kd
		PID_init(kp,ki,kd,skap);//PID控制初始化|参数-->|比例Kp|积分Ki|微分Kd|空占比skap
	}
}


/**
*		函数名：Mode_Switch
*		作用：在设定速度和设定角度模式中进行转换
*		入口参数：uchar型模式标志
*		返回值：无
*		修改日期：2019年8月11日14:11:25
*/

/**
*		模式选择函数
*		0：模式2 1：模式1
**********************/
void Mode_Switch(u8 mode)
{
	if(mode)
	{
		setspeed = EEPROM_Read(0x00);
		kp = EEPROM_ReadFloat(0x03);//初始化kp
		ki = EEPROM_ReadFloat(0x0B);//初始化ki
		kd = EEPROM_ReadFloat(0x13);//初始化kd
		PID_init(kp,ki,kd,skap);//PID控制初始化|参数-->|比例Kp|积分Ki|微分Kd|空占比skap
	}else
	{
		setspeed = 0;
		kp = EEPROM_ReadFloat(0x23);//初始化kp
		ki = EEPROM_ReadFloat(0x2B);//初始化ki
		kd = EEPROM_ReadFloat(0x33);//初始化kd
		PID_init(kp,ki,kd,skap);//PID控制初始化|参数-->|比例Kp|积分Ki|微分Kd|空占比skap
	}
}


void Key_function(u8 key)
{
	int buff;
	
	switch(key)//按键反馈
	{
		case 1:setspeed++;EEPROM_Write(0x00,setspeed);EP_FLAG |= 0x01;EEPROM_Write(0x1B,EP_FLAG);delay_ms(50);break;
		//增加设定的速度|每次增加都将刷新EEPROM的对应内存地址
		case 2:setangle_buff++;EEPROM_Write(0x01,setangle_buff);EP_FLAG |= 0x02;EEPROM_Write(0x1B,EP_FLAG);delay_ms(50);break;
		//增加设定的角度|每次增加都将刷新EEPROM的对应内存地址
		case 3:buff = skap;skap = 0;delay_ms(100);skap = buff;Dir_rand = Motor_dir(1);EEPROM_Write(0x02,Dir_rand);EP_FLAG |= 0x04;EEPROM_Write(0x1B,EP_FLAG);break;
		//设置电机为正传|每次按下都将刷新EEPROM的对应内存地址
		case 4:Apply(Mode);delay_ms(50);break;
		//应用更改|模式1下应用对 PID参数 的更改|模式2下应用对 角度|PID参数 的更改
		case 5:setspeed--;EEPROM_Write(0x00,setspeed);EP_FLAG |= 0x01;delay_ms(50);EEPROM_Write(0x1B,EP_FLAG);break;
		//降低设定的速度|每次降低都将刷新EEPROM的对应内存地址
		case 6:setangle_buff--;EEPROM_Write(0x01,setangle_buff);EP_FLAG |= 0x02;delay_ms(50);EEPROM_Write(0x1B,EP_FLAG);break;
		//降低设定的角度|每次降低都将刷新EEPROM的对应内存地址
		case 7:buff = skap;skap = 0;delay_ms(100);skap = buff;Dir_rand = Motor_dir(0);EEPROM_Write(0x02,Dir_rand);EP_FLAG |= 0x04;EEPROM_Write(0x1B,EP_FLAG);break;
		//设置电机反转|每次按下都将刷新EEPROM的对应内存地址
		case 8:if(mp>0.00001) mp *= 0.1;else mp = 1.0;delay_ms(50);break;
		//调整PID调整系数|每次按下减小10倍
		case 9:kp += 1 * mp;if(Mode){EEPROM_WriteFloat(0x03,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x23,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
		//根据调整系数增加KP|根据模式存放数据于对应地址
		case 10:Mode = !Mode;Mode_Switch(Mode);delay_ms(100);break;
		//设定 速度|角度 模式切换	
		case 11:ki += 1 * mp;if(Mode){EEPROM_WriteFloat(0x0B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x2B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
		//根据调整系数增加KI|根据模式存放数据于对应地址
		case 12:kd += 1 * mp;if(Mode){EEPROM_WriteFloat(0x13,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x33,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
		//根据调整系数增加KD|根据模式存放数据于对应地址
		case 13:kp -= 1 * mp;if(Mode){EEPROM_WriteFloat(0x03,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x23,kp);EP_FLAG |= 0x08;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
		//根据调整系数减少KP|根据模式存放数据于对应地址
		case 14:/*保留*/;break;
		//保留|此按键损坏
		case 15:ki -= 1 * mp;if(Mode){EEPROM_WriteFloat(0x0B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x2B,ki);EP_FLAG |= 0x10;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
		//根据调整系数减少KI|根据模式存放数据于对应地址
		case 16:kd -= 1 * mp;if(Mode){EEPROM_WriteFloat(0x13,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}else{EEPROM_WriteFloat(0x33,kd);EP_FLAG |= 0x20;EEPROM_Write(0x1B,EP_FLAG);}delay_ms(50);break;
		//根据调整系数减少KD|根据模式存放数据于对应地址
		default:break;
	}
}

void SpeedWarning_function(void)//失速报警函数
{
	if((setspeed-Motor_Read_Speed()>20||setspeed-Motor_Read_Speed()<-20) && Mode)//误差超过20并且在模式1下报警
	{
		BEEP = 1;
	}else 
	{
		BEEP = 0;
	}
}

void SpeedLimi_function(void)//速度限制函数
{
	/*之前是25-70|因为调整角度时速度可以达到0，故更改，但是实际速度范围仍然为25-30*/
	if(setspeed>70)
	{
		setspeed = 70;
	}else if(setspeed<0)
	{
		setspeed = 0;
	}
}

void SkipLimit_function(void)//空占比限制函数
{
	if(skap>999)
	{
		skap = 999;
	}else if(skap<350)
	{
		skap = 350;
	}
}

void InitializeByEEPROM_function(void)//从EEPROM初始化函数
{
	EP_FLAG = EEPROM_Read(0x1B);//获取标志位,根据标志位初始化
	if(( EP_FLAG&0x01) == 0x01) setspeed = EEPROM_Read(0x00);//初始化速度
	if(( EP_FLAG&0x02) == 0x02) setangle_buff = EEPROM_Read(0x01);//初始化角度
	if(( EP_FLAG&0x04) == 0x04) Dir_rand = Motor_dir(EEPROM_Read(0x02));//初始化方向
	if(Mode)//从EEPROM PID参数（速度）初始化PID参数
	{
		if(( EP_FLAG&0x08) == 0x08) kp = EEPROM_ReadFloat(0x03);//初始化kp
		if(( EP_FLAG&0x10) == 0x10) ki = EEPROM_ReadFloat(0x0B);//初始化ki
		if(( EP_FLAG&0x20) == 0x20) kd = EEPROM_ReadFloat(0x13);//初始化kd
	}else//从EEPROM PID参数（角度）初始化PID参数
	{
		setspeed = 0;
		if(( EP_FLAG&0x08) == 0x08) kp = EEPROM_ReadFloat(0x23);//初始化kp
		if(( EP_FLAG&0x10) == 0x10) ki = EEPROM_ReadFloat(0x2B);//初始化ki
		if(( EP_FLAG&0x20) == 0x20) kd = EEPROM_ReadFloat(0x33);//初始化kd
	}
}
