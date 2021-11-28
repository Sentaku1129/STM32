#include "SysTick.h"
#include "core_cm3.h"
#include "misc.h"

static __IO uint32_t TimingDelay;

//中断
void Delay_Ms(__IO uint32_t nTime)
{
	SysTick_Config(SystemCoreClock/1000);
	TimingDelay = nTime;	

	// 使能滴答定时器  
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

	while(TimingDelay != 0);
}


//SysTick_Handler
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0)
	{ 
		TimingDelay--;
	}
}

//轮询
#if 0
void SysTick_Delay_Ms(__IO uint32_t ms)
{
	uint32_t i;	
	SysTick_Config(SystemCoreClock/1000);
	
	for(i=0;i<ms;i++)
	{
		// 当计数器的值减小到0的时候，CRTL寄存器的位16会置1
		// 当置1时，读取该位会清0
		while(!((SysTick->CTRL)&(1<<16)));
	}
	// 关闭SysTick定时器
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
}

#else
void SysTick_Delay_Ms(__IO uint32_t ms)
{
	uint32_t i;	
	
	SysTick->CTRL = 0;                            										 //禁止SysTick
	SysTick->LOAD = ((SystemCoreClock/1000)&SysTick_LOAD_RELOAD_Msk);  //计数范围SystemCoreClock/1000 => 1ms
	SysTick->VAL = 0;                             										 //清除当前值和计数标记
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk|SysTick_CTRL_ENABLE_Msk;//配置处理器时钟并使能SysTick定时器
	for(i=0;i<ms;i++)
	{
		// 当计数器的值减小到0的时候，CRTL寄存器的位16会置1
		// 当置1时，读取该位会清0
		while(!((SysTick->CTRL)&(1<<16)));
	}    									 //等待计数标志置位
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;                          //禁止SysTick
}
#endif
