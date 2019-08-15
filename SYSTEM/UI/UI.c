#include "UI.h"
#include "lcd.h"
#include "motor.h"

/*ȫ�ֱ�������|ÿ�ν�����Щֵ��ϣ������ʼ��*/
int x=5,y =150;//�ٶ�ʱ��ͼ�񱾴ε�
int last_x = 5,last_y = 150;//�ٶ�ʱ��ͼ���ϴε�
int last_ay = 280;//�Ƕ�ʱ��ͼ���ϴε�
int last_sety = 150;
char kp0[8],ki0[8],kd0[8];//8λ������PID����ת��ΪChar�͵�ֵ
char mp0[8];//���PID��������ϵ��ת��ΪChar��������ֵ

int angle_y = 279;

extern u8 Mode;

/**
*		��������float_to_char
*		���ã���С�����5λ��float�����ݱ�Ϊchar�ʹ����ڴ����������
*		��ڲ�����float�͵�ԭʼ���� 8��Ԫ�ص�char������
*		����ֵ����
*		�޸����ڣ�2019��8��10��21:24:48
*/

void float_to_char(float number,char buff[8])
{
	buff[0] = (int)number%10+48;
	buff[1] = '.';
	buff[2] = (int)(number*10)%10+48;
	buff[3] = (int)(number*100)%10+48;
	buff[4] = (int)(number*1000)%10+48;
	buff[5] = (int)(number*10000)%10+48;
	buff[6] = (int)(number*100000)%10+48;
	buff[7] = '\0';
}

/**
*		UIˢ�º���
*		ҳ��Ԫ�أ�
*		�߿�1			����  Setspeed Setangle �� ������ʾ��
*		�߿�2			����	Speed Angle Skap
*		�߿�3			����	��̬ͼ�� Ŀ���ٶ��� ��ǰ�ٶȹ���ʱ��ĺ���ͼ��
*		�߿�4			����	��̬ͼ�� ��ǰ�Ƕȹ���ʱ��ĺ���ͼ��
*		�߿�5			����	Kp Ki Kd Key MP �����󱨾���ʾ��
*/

void UI_Working(void)
{
	/*�ⲿ��������*/
	extern int setspeed;//�ٶ��趨ֵ
	extern int setangle_buff;//�Ƕ��趨ֵ
	extern int skap;
	extern u8 Dir_rand;//ת���־��
	extern u8 key;//������ȡֵ
	extern float kp,ki,kd;//PID����
	extern float mp;
	
	/*��float�Ͳ���ת��Ϊchar�ʹ�����������*/
	float_to_char(kp,kp0);
	float_to_char(ki,ki0);
	float_to_char(kd,kd0);
	float_to_char(mp,mp0);
	
	
	/*���߿�*/
	POINT_COLOR=BLUE;
	LCD_DrawRectangle(5,5,235,30);//�߿�1
	
	POINT_COLOR=BLACK;
	LCD_DrawRectangle(5,35,235,60);//�߿�2
	
	POINT_COLOR=MAGENTA;
	LCD_DrawRectangle(5,65,235,155);//�߿�3
	
	POINT_COLOR=MAGENTA;
	LCD_DrawRectangle(5,160,235,280);//�߿�4
	
	POINT_COLOR=BLUE;
	LCD_DrawRectangle(5,285,135,310);//�߿�5
	
	
	/*�������*/
	POINT_COLOR=RED;
	LCD_ShowString(10,10,200,16,16,"Setspeed:");
	LCD_ShowxNum(80,10,setspeed,3,16,0);
	LCD_ShowString(110,10,200,16,16,"Setangle:");
	LCD_ShowxNum(180,10,setangle_buff,3,16,0);
	
	if(Dir_rand==0)
		LCD_ShowChar(215,10,82,16,0);
	else
		LCD_ShowChar(215,10,76,16,0);
	
	LCD_ShowString(10,40,200,16,16,"Speed:");
	LCD_ShowxNum(58,40,Motor_Read_Speed(),3,16,0);
	LCD_ShowString(86,40,200,16,16,"Angle:");
	LCD_ShowxNum(132,40,Motor_Read_Angle(),3,16,0);
	LCD_ShowString(160,40,200,16,16,"Skap:");
	LCD_ShowxNum(200,40,skap,4,16,0);
	
	LCD_ShowString(7,286,200,12,12,"Kp:");
	LCD_ShowString(25,286,200,12,12,kp0);
	LCD_ShowString(7,298,200,12,12,"Ki:");
	LCD_ShowString(25,298,200,12,12,ki0);
	LCD_ShowString(75,286,200,12,12,"Kd:");
	LCD_ShowString(93,286,200,12,12,kd0);
	POINT_COLOR=BLUE;
	LCD_ShowString(82,298,200,12,12,"Key:");
	LCD_ShowNum(106,298,key,2,12);
	POINT_COLOR=BLACK;
	LCD_ShowString(142,286,200,12,12,"MP:");
	LCD_ShowString(160,286,200,12,12,mp0);
	
	/*�ٶ�\�Ƕȶ���ʱ��ĺ���ͼ�����*/
	POINT_COLOR=BLACK;
	if(x<235)
	{
		x++;
		/*���ƻ滭�߽�*/
		if(150-Motor_Read_Speed()<67)
			y = 67;
		else if(150-Motor_Read_Speed()>150)
			y = 150;
		else
			y = 150-Motor_Read_Speed();
		
		/*�ٶȻ���|˫��*/
		LCD_DrawLine(last_x,last_y,x,y);
		LCD_DrawLine(last_x,last_y-1,x,y-1);
		last_x = x;
		last_y = y;
		
		/*�ǶȻ���|˫��*/
		LCD_DrawLine(last_x,last_ay,x,280-(int)Motor_Read_Angle()/3.3);
		last_ay = 280-(int)Motor_Read_Angle()/3.3;
		
		/*���Ƕȳ������� ��������*/
		/*�����ʲ��������ص�����*/
		
	}else//������򳬳����Ʊ߿�
	{
		LCD_Fill(5,65,235,155,YELLOW);//����
		LCD_Fill(5,160,235,280,YELLOW);
		
		POINT_COLOR=MAGENTA;
		LCD_DrawRectangle(5,65,235,155);
	
		POINT_COLOR=MAGENTA;
		LCD_DrawRectangle(5,160,235,280);
		
		/*��ʼ��������ֵ*/
		last_x = 5;
		x = 5;
	}
	
	/*�ٶ�ָʾ�߻���*/
	POINT_COLOR=YELLOW;
	LCD_DrawLine(5,last_sety,235,last_sety);
	POINT_COLOR=GREEN;
	LCD_DrawLine(5,150-setspeed,235,150-setspeed);
	last_sety = 150-setspeed;
	
	/*�Ƕ�ָʾ�߻���*/
	POINT_COLOR=YELLOW;
	LCD_DrawLine(5,angle_y,235,angle_y);
	POINT_COLOR=GREEN;
	LCD_DrawLine(5,279-setangle_buff/3.3,235,279-setangle_buff/3.3);
	angle_y = 279-setangle_buff/3.3;
	
	/*�����󱨾���־|����20*/
	if(setspeed-Motor_Read_Speed()>20||setspeed-Motor_Read_Speed()<-20)//����20�ı���
	{
		POINT_COLOR=RED;
		LCD_ShowString(145,298,200,12,12,"Warning!");
	}else 
	{
		LCD_Fill(144,297,235,310,YELLOW);
	}
	
	/*ģʽָʾ*/
	POINT_COLOR=RED;
	if(Mode)
		LCD_ShowChar(210,286,83,24,0);
	else
		LCD_ShowChar(210,286,65,24,0);
	
	
}

