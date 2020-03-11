#ifndef _TIMER_H
#define _TIMER_H

#include "sys.h"

void timer2_init(void);
void TIM2_IRQHandler(void);
void TimerReset(TIM_TypeDef* TIMx, uint16_t time);

#endif
