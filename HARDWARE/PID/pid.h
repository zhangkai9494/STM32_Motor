#ifndef __PID_H_
#define __PID_H_

#include "sys.h"


void PID_init(float Kp,float Ki,float Kd,u16 skap);//PID³õÊ¼»¯º¯Êý
int PID_realize(float speed,float ActualSpeed);
int PID_Angle(float setangle,float angle);


#endif
