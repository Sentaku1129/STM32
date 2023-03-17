#ifndef __SYSTICK_H_
#define __SYSTICK_H_

#include "stm32f10x.h"

void	SysTick_Init(u8 SYSTICK);
void delay_us(u32 nus);
void delay_ms(u32 nms);
void delay_xms(u32 nms);


#endif
