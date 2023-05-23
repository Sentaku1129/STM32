#include "stm32f10x.h"
#include "SysTick.h"

void PWM_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // ʹ�ܶ�ʱ��3��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // ����GPIO����Ϊ���ù���
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // ʹ��PA6��PA7������ΪPWM���
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ���ö�ʱ��3
    TIM_TimeBaseStructure.TIM_Period = 999;  // PWM�źŵ�����Ϊ1000��ʱ������
    TIM_TimeBaseStructure.TIM_Prescaler = 71; // ʱ��Ԥ��ƵΪ72���õ�1MHz��ʱ��
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // ����PWMͨ��1��ͨ��2
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // ����PWMͨ��1
    TIM_OCInitStructure.TIM_Pulse = 500;  // PWM�źŵ�ռ�ձ�Ϊ50%
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // ����PWMͨ��2
    TIM_OCInitStructure.TIM_Pulse = 800;  // PWM�źŵ�ռ�ձ�Ϊ80%
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // ������ʱ��3
    TIM_Cmd(TIM3, ENABLE);
}

int main(void)
{
    // ��ʼ��ϵͳʱ�ӵ�
	int i;
    // ����PWM
    PWM_Configuration();

    while (1)
    {
		Delay_Us(1000);
		for(i=0;i<900;i++)
		{
			TIM_SetCompare1(TIM3, 1000-i);
			TIM_SetCompare2(TIM3, i);
			Delay_Us(1000);
		}
        // �����������������Ĵ����߼�
    }
}
