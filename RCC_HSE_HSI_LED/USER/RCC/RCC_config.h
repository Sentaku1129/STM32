#ifndef __RCC_CONFIG_H
#define	__RCC_CONFIG_H


#include "stm32f10x.h"

void HSE_SetSysClock(uint32_t pllmul);
void HSI_SetSysClock(uint32_t pllmul);
void MCO_GPIO_Config(void);

#endif
