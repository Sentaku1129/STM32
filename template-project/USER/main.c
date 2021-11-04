#include "stm32f10x.h"

#define LED_ON GPIO_SetBits(GPIOD, GPIO_Pin_13)        //端口置1
#define LED_OFF GPIO_ResetBits(GPIOD, GPIO_Pin_13)     //端口置0

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

void LED_GPIO_Config(void);
void delay(u32 t);

void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//打开外设时钟
	RCC_APB2PeriphClockCmd(LED3_GPIO_CLK,ENABLE);
	//配置初始化结构体
	GPIO_InitStruct.GPIO_Pin = LED3_GPIO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	//调用外设初始化函数，把配置好的结构体成员写到寄存器里面
	GPIO_Init(LED1_GPIO_PORT,&GPIO_InitStruct);
}

void delay(u32 t)            //延时函数
{
    u16 i;
    while(t--)
        for(i=0;i<1000;i++);
}

int main()
{
    LED_GPIO_Config();            //GPIOD_1初始化配置
    while(1)
    {
        LED_ON;                 //点亮
        delay(1000);						//延时大概几百毫秒
        LED_OFF;                //熄灭
        delay(1000);
    }

}
