#include "stm32f10x.h"

#define R_LED GPIO_Pin_5
#define G_LED GPIO_Pin_0
#define B_LED GPIO_Pin_1

#define KEY1  GPIO_Pin_0 

void LED_GPIO_Config()
{
		/*定义一个GPIO_InitTypeDef类型的结构体*/
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启LED相关的GPIO外设时钟*/
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		/*选择要控制的GPIO引脚*/
		GPIO_InitStructure.GPIO_Pin = R_LED|B_LED|G_LED;
		/*设置引脚模式为通用推挽输出*/
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		/*设置引脚速率为50MHz */   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		/*调用库函数，初始化GPIOB*/
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		/* 关闭所有led灯	*/
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
		/* 关闭所有led灯	*/
		GPIO_SetBits(GPIOB, GPIO_Pin_1);	 
    /* 关闭所有led灯	*/
		GPIO_SetBits(GPIOB, GPIO_Pin_5);
}

void KEY_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure; //构造结构体 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能其所在时钟        
    GPIO_InitStructure.GPIO_Pin = KEY1;//定义IO端口                       
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//定义IO端口模式为浮空输入      
  
    GPIO_Init(GPIOA,&GPIO_InitStructure);
}


void delay(u32 t)            //延时函数
{
    u16 i;
    while(t--)
        for(i=0;i<1000;i++);
}

int main()
{
	uint16_t s = 0;
	LED_GPIO_Config();            //GPIOD初始化配置
	KEY_Init();										//KEY1 初始化 
	while(1)
	{
		if(GPIO_ReadInputDataBit(GPIOA,KEY1))
		{
			delay(2000);
			if(GPIO_ReadInputDataBit(GPIOA,KEY1))
			{
				s = s+1;
			}
		}
		if(s%2 == 0)
		{
			GPIO_SetBits(GPIOB, G_LED);
			GPIO_SetBits(GPIOB, B_LED);
			GPIO_ResetBits(GPIOB, R_LED);
		}
		if(s%2 == 1)
		{
			GPIO_SetBits(GPIOB, G_LED);
			GPIO_SetBits(GPIOB, R_LED);
			GPIO_ResetBits(GPIOB, B_LED);
		}
	}
}
