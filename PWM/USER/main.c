#include "stm32f10x.h"
#include "SysTick.h"

void PWM_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 使能定时器3和GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置GPIO引脚为复用功能
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;  // 使用PA6和PA7引脚作为PWM输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置定时器3
    TIM_TimeBaseStructure.TIM_Period = 999;  // PWM信号的周期为1000个时钟周期
    TIM_TimeBaseStructure.TIM_Prescaler = 71; // 时钟预分频为72，得到1MHz的时钟
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // 配置PWM通道1和通道2
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    // 配置PWM通道1
    TIM_OCInitStructure.TIM_Pulse = 500;  // PWM信号的占空比为50%
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 配置PWM通道2
    TIM_OCInitStructure.TIM_Pulse = 800;  // PWM信号的占空比为80%
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

    // 启动定时器3
    TIM_Cmd(TIM3, ENABLE);
}

int main(void)
{
    // 初始化系统时钟等
	int i;
    // 配置PWM
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
        // 在这里可以添加其他的代码逻辑
    }
}
