#include "interrupt.h"
#include "usart.h"

void TIM6_Init(uint16_t per,uint16_t psc)
{
		NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);		//ʹ��TIM6ʱ��
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;				//TIM6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;	//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	TIM_TimeBaseStructure.TIM_Period = per-1;					//�ܵ�ֵ����Ϊ0xFFFF���趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;				//Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);				//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_Cmd(TIM6, ENABLE);										//������ʱ��
	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);					//������ʱ�������ж�
}

volatile uint32_t CPU_RunTime = 0UL;

void TIM6_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update))		//���ָ����TIM�жϷ������:TIM �ж�Դ
	{
		CPU_RunTime++;
	}
	TIM_ClearITPendingBit(TIM6,TIM_IT_Update);	//���TIMx���жϴ�����λ:TIM �ж�Դ 
}

