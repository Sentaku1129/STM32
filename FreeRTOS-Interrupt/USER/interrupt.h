#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include "stdint.h"

void TIM4_Init(uint16_t per,uint16_t psc);
void TIM3_Init(uint16_t per,uint16_t psc);
void TIM4_IRQHandler(void);
void TIM3_IRQHandler(void);

#endif
