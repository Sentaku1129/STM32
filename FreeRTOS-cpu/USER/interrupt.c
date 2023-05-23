#include "interrupt.h"
#include "usart.h"

void TIM6_Init(uint16_t per,uint16_t psc)
{
		NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);		//使能TIM6时钟
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;				//TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;	//抢占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);								//根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	
	TIM_TimeBaseStructure.TIM_Period = per-1;					//总的值设置为0xFFFF，设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;				//预分频器
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);				//根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	TIM_Cmd(TIM6, ENABLE);										//开启定时器
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);					//开启定时器更新中断
}

volatile uint32_t CPU_RunTime = 0UL;

void TIM6_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update))		//检查指定的TIM中断发生与否:TIM 中断源
	{
		CPU_RunTime++;
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);	//清除TIMx的中断待处理位:TIM 中断源 
}

