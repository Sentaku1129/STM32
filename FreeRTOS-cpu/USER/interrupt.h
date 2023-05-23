#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include "stdint.h"

void TIM6_Init(uint16_t per,uint16_t psc);
void TIM6_IRQHandler(void);

#endif
