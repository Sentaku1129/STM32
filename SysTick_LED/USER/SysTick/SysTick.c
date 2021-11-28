#include "SysTick.h"
#include "core_cm3.h"
#include "misc.h"

static __IO uint32_t TimingDelay;

//�ж�
void Delay_Ms(__IO uint32_t nTime)
{
	SysTick_Config(SystemCoreClock/1000);
	TimingDelay = nTime;	

	// ʹ�ܵδ�ʱ��  
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

//��ѯ
void SysTick_Delay_Ms( __IO uint32_t ms)
{
	uint32_t i;	
	SysTick_Config(SystemCoreClock/1000);
	
	for(i=0;i<ms;i++)
	{
		// ����������ֵ��С��0��ʱ��CRTL�Ĵ�����λ16����1
		// ����1ʱ����ȡ��λ����0
		while(!((SysTick->CTRL)&(1<<16)));
	}
	// �ر�SysTick��ʱ��
	SysTick->CTRL &=~ SysTick_CTRL_ENABLE_Msk;
}
