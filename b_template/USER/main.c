#include "stm32f10x.h"

/*
#define LED_ON GPIO_SetBits(GPIOD, GPIO_Pin_13)        //端口置1
#define LED_OFF GPIO_ResetBits(GPIOD, GPIO_Pin_13)     //端口置0
*/

//R-红色
#define LED1_GPIO_PORT GPIOB
#define LED1_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED1_GPIO_PIN GPIO_Pin_5
//G-绿色
#define LED2_GPIO_PORT GPIOB
#define LED2_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED2_GPIO_PIN GPIO_Pin_0
// B-蓝色
#define LED3_GPIO_PORT GPIOB
#define LED3_GPIO_CLK RCC_APB2Periph_GPIOB
#define LED3_GPIO_PIN GPIO_Pin_1

void delay(u32 t);
void ON_to_OFF(uint16_t GPIO_Pin);
void LED_GPIO_Config();

void LED_GPIO_Config()
{
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启LED相关的GPIO外设时钟*/
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		RCC_APB2PeriphClockCmd( LED1_GPIO_CLK | LED2_GPIO_CLK | LED3_GPIO_CLK, ENABLE);
	
		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN;
		
		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		/*调用库函数，初始化GPIO*/
		GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);	
		
		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;

		/*调用库函数，初始化GPIO*/
		GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);
		
		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = LED3_GPIO_PIN;

		/*调用库函数，初始化GPIOF*/
		GPIO_Init(LED3_GPIO_PORT, &GPIO_InitStructure);
	
		/* 关闭所有led灯	*/
		GPIO_SetBits(LED1_GPIO_PORT, LED1_GPIO_PIN);
		
		/* 关闭所有led灯	*/
		GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);	 
    
    /* 关闭所有led灯	*/
		GPIO_SetBits(LED3_GPIO_PORT, LED3_GPIO_PIN);
}

void delay(u32 t)            //延时函数
{
    u16 i;
    while(t--)
        for(i=0;i<1000;i++);
}

void ON_to_OFF(uint16_t GPIO_Pin)
{
	GPIO_SetBits(GPIOB, GPIO_Pin);
	delay(10000);
	GPIO_ResetBits(GPIOB, GPIO_Pin);
	delay(10000);
}

int main()
{
		LED_GPIO_Config();            //GPIOD初始化配置
    while(1)
    {
        ON_to_OFF(GPIO_Pin_0);
				ON_to_OFF(GPIO_Pin_13);
				ON_to_OFF(GPIO_Pin_5);
    }

}
