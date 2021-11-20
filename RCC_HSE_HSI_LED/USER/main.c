#include "stm32f10x.h"
#include "RCC_config.h"

//LED GPIO_pin
#define R_LED GPIO_Pin_5
#define G_LED GPIO_Pin_0
#define B_LED GPIO_Pin_1

#define LED_ON GPIO_SetBits(GPIOB, R_LED)        //端口置1
#define LED_OFF GPIO_ResetBits(GPIOB, R_LED)     //端口置0

void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//打开外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//配置初始化结构体
	GPIO_InitStruct.GPIO_Pin = R_LED|G_LED|B_LED;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	//调用外设初始化函数，把配置好的结构体成员写到寄存器里面
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	//关闭三个RGB三个LED
	GPIO_SetBits(GPIOB, R_LED);
	GPIO_SetBits(GPIOB, G_LED);
	GPIO_SetBits(GPIOB, B_LED);
}

void Delay(__IO uint32_t nCount)	
{
	for(; nCount != 0; nCount--);
}

int main()
{
  LED_GPIO_Config();            //LED初始化配置
	HSE_SetSysClock(RCC_PLLMul_8);
	//HSI_SetSysClock(RCC_PLLMul_16);
	
	/******设置PA8输出初始化******/
	// MCO 引脚初始化
	MCO_GPIO_Config();
	
	// MCO引脚输出可以是HSE,HSI,PLLCLK/2,SYSCLK	
	//RCC_MCOConfig(RCC_MCO_HSE);	             	        
	//RCC_MCOConfig(RCC_MCO_HSI);	                   
	//RCC_MCOConfig(RCC_MCO_PLLCLK_Div2);    	
	//RCC_MCOConfig(RCC_MCO_SYSCLK);
	
	while (1)
	{
		LED_ON;			  // 亮
		Delay(0x0FFFFF);
		LED_OFF;		  // 灭 
		Delay(0x0FFFFF);		
	}
}
