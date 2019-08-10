#ifndef __TIMER_H_
#define __TIMER_H_

#include "sys.h"

void TIM3_PWM_Init(u16 arr,u16 psc);
void TIM5_Cap_Init(u16 arr,u16 psc);
void TIM4_PID_Init(u16 arr,u16 psc);

#endif
