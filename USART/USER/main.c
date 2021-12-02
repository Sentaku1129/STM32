#include "stm32f10x.h"
#include "usart.h"
#include <string.h>

#define R_LED GPIO_Pin_5
#define G_LED GPIO_Pin_0
#define B_LED GPIO_Pin_1

#define BEEP GPIO_Pin_8

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
	
	GPIO_SetBits(GPIOB,R_LED|G_LED|B_LED);
}

void BEEP_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//打开外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	//配置初始化结构体
	GPIO_InitStruct.GPIO_Pin = BEEP;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	//调用外设初始化函数，把配置好的结构体成员写到寄存器里面
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_SetBits(GPIOA,BEEP);
	GPIO_ResetBits(GPIOA,BEEP);
}

void LED_ON_OFF(char *LED)
{
	//红、绿、蓝、黄、紫、青、白、黑、蜂鸣器开、蜂鸣器关
	if(!strcmp(LED,"RED"))
	{
		GPIO_SetBits(GPIOB,G_LED|B_LED);
		GPIO_ResetBits(GPIOB,R_LED);
	}
	else if(!strcmp(LED,"GREEN"))
	{
		GPIO_SetBits(GPIOB,R_LED|B_LED);
		GPIO_ResetBits(GPIOB,G_LED);
	}
	else if(!strcmp(LED,"BLUE"))
	{
		GPIO_SetBits(GPIOB,G_LED|R_LED);
		GPIO_ResetBits(GPIOB,B_LED);
	}
	else if(!strcmp(LED,"YELLOW"))
	{
		GPIO_SetBits(GPIOB,B_LED);
		GPIO_ResetBits(GPIOB,G_LED|R_LED);
	}
	else if(!strcmp(LED,"PURPLE"))
	{
		GPIO_SetBits(GPIOB,G_LED);
		GPIO_ResetBits(GPIOB,B_LED|R_LED);
	}
	else if(!strcmp(LED,"BLUENESS"))
	{
		GPIO_SetBits(GPIOB,R_LED);
		GPIO_ResetBits(GPIOB,B_LED|G_LED);
	}
	else if(!strcmp(LED,"WHITE"))
	{
		GPIO_ResetBits(GPIOB,G_LED|R_LED|B_LED);
	}
	else if(!strcmp(LED,"BLACK"))
	{
		GPIO_SetBits(GPIOB,G_LED|R_LED|B_LED);
	}
	else if(!strcmp(LED,"BEEP_ON"))
	{
		GPIO_SetBits(GPIOA,BEEP);
	}
	else if(!strcmp(LED,"BEEP_OFF"))
	{
		GPIO_ResetBits(GPIOA,BEEP);
	}
}

int main()
{
	char ch;
  LED_GPIO_Config();            //LED初始化配置
	BEEP_GPIO_Config();						//蜂鸣器初始化配置
	USART_Config();								//串口初始化
	
	while(1)
	{
		ch = getchar();
		//GPIO_ResetBits(GPIOB,B_LED);
		printf("接收到字符：%c\n",ch);
		switch(ch)
		{
			case '1':LED_ON_OFF("RED");break;
			case '2':LED_ON_OFF("GREEN");break;
			case '3':LED_ON_OFF("BLUE");break;
			case '4':LED_ON_OFF("YELLOW");break;
			case '5':LED_ON_OFF("PURPLE");break;
			case '6':LED_ON_OFF("BLUENESS");break;
			case '7':LED_ON_OFF("WHITE");break;
			case '8':LED_ON_OFF("BLACK");break;
			case '9':LED_ON_OFF("BEEP_ON");break;
			case 'a':LED_ON_OFF("BEEP_OFF");break;
		}
	}
}
