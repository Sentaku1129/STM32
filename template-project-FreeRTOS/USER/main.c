#include "stm32f10x.h"
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"

#define R_LED GPIO_Pin_5
#define G_LED GPIO_Pin_0
#define B_LED GPIO_Pin_1

#define KEY1  GPIO_Pin_0 
#define KEY2  GPIO_Pin_13

//任务优先级
#define START_TASK_PRIO 1
//任务堆栈大小
#define START_STK_SIZE 128
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);


//任务优先级
#define LED1_TASK_PRIO 2
//任务堆栈大小
#define LED1_STK_SIZE 50
//任务句柄
TaskHandle_t led1Task_Handler;
//任务函数
void led1_task(void *pvParameters);


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


int main()
{
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	LED_GPIO_Config();            	//GPIOD初始化配置
	
	xTaskCreate((TaskFunction_t	)start_task, 					//任务函数
							(const char *		)"start_task",				//任务名称
							(uint16_t				)START_STK_SIZE,			//任务堆栈大小
							(void *					)NULL,								//传递给任务函数的参数
							(UBaseType_t		)START_TASK_PRIO,			//任务优先级
							(TaskHandle_t *	)&StartTask_Handler);	//任务句柄
	vTaskStartScheduler(); 	//开始任务调度
}

void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();								//进入临界区
	
	//创建LED1任务函数
	xTaskCreate((TaskFunction_t	)led1_task, 					//任务函数
							(const char *		)"led1_task",					//任务名称
							(uint16_t				)LED1_STK_SIZE,				//任务堆栈大小
							(void *					)NULL,								//传递给任务函数的参数
							(UBaseType_t		)START_TASK_PRIO,			//任务优先级
							(TaskHandle_t *	)&led1Task_Handler);	//任务句柄
							
	vTaskDelete(StartTask_Handler);			//删除开始任务
	taskEXIT_CRITICAL();								//退出临界区
}

//LED1任务函数
void led1_task(void *pvParameters)
{
	while(1)
	{
		GPIO_SetBits(GPIOB,B_LED);
		vTaskDelay(500);
		GPIO_ResetBits(GPIOB,B_LED);
		vTaskDelay(500);
	}
}

