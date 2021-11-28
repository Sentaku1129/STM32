#ifndef __SYSTICK_H_
#define __SYSTICK_H_

#include "stm32f10x.h"

void Delay_Ms(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
void SysTick_Delay_Ms(__IO uint32_t ms);


#endif
