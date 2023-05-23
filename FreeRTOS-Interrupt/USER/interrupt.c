#include "interrupt.h"
#include "usart.h"

void TIM4_Init(u16 per,u16 psc)
{
		NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);		//ʹ��TIM4ʱ��
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;				//TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;	//��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	TIM_TimeBaseStructure.TIM_Period = per-1;					//�ܵ�ֵ����Ϊ0xFFFF���趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;				//Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);				//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_Cmd(TIM4, ENABLE);										//������ʱ��
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);					//������ʱ�������ж�
}

void TIM3_Init(u16 per,u16 psc)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		//ʹ��TIM3ʱ��
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;				//TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;	//��ռ���ȼ�4��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			//�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);								//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	TIM_TimeBaseStructure.TIM_Period = per-1;					//�ܵ�ֵ����Ϊ0xFFFF���趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;				//Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);				//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	
	TIM_Cmd(TIM3, ENABLE);										//������ʱ��
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);					//������ʱ�������ж�
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update))		//���ָ����TIM�жϷ������:TIM �ж�Դ
	{
		printf("TIM4���......\r\n");
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	//���TIMx���жϴ�����λ:TIM �ж�Դ
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update))		//���ָ����TIM�жϷ������:TIM �ж�Դ
	{
		printf("TIM3���......\r\n");		
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	//���TIMx���жϴ�����λ:TIM �ж�Դ 
}

