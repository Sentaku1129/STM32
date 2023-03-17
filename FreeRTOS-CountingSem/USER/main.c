#include "stm32f10x.h"
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "usart.h"
#include "key.h"

#define R_LED GPIO_Pin_5
#define G_LED GPIO_Pin_0
#define B_LED GPIO_Pin_1



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

//任务优先级
#define SEND_TASK_PRIO 3
//任务堆栈大小
#define SEND_STK_SIZE 50
//任务句柄
TaskHandle_t send_Task_Handler;
//任务函数
void send_task(void *pvParameters);

//任务优先级
#define ACCEPT_TASK_PRIO 4
//任务堆栈大小
#define ACCEPT_STK_SIZE 50
//任务句柄
TaskHandle_t accept_Task_Handler;
//任务函数
void accept_task(void *pvParameters);

//计数信号量句柄
SemaphoreHandle_t CountingSem_Handle = NULL;


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
	KEY_GPIO_Config();				//KEY1,KEY2初始化配置
	USART_Config();					//串口初始化配置
	
	xTaskCreate((TaskFunction_t	)start_task, 								//任务函数
							(const char *		)"start_task",				//任务名称
							(uint16_t				)START_STK_SIZE,		//任务堆栈大小
							(void *					)NULL,					//传递给任务函数的参数
							(UBaseType_t		)START_TASK_PRIO,			//任务优先级
							(TaskHandle_t *	)&StartTask_Handler);			//任务句柄
	vTaskStartScheduler(); 	//开始任务调度
}

void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();								//进入临界区

	/* 创建计数信号量 */
	CountingSem_Handle = xSemaphoreCreateCounting(5,5);

	//创建LED1任务函数
	xTaskCreate((TaskFunction_t	)led1_task, 						//任务函数
							(const char *		)"led1_task",		//任务名称
							(uint16_t				)LED1_STK_SIZE,	//任务堆栈大小
							(void *					)NULL,			//传递给任务函数的参数
							(UBaseType_t		)START_TASK_PRIO,	//任务优先级
							(TaskHandle_t *	)&led1Task_Handler);	//任务句柄
	//创建信号量发送释放任务函数
	xTaskCreate((TaskFunction_t	)send_task, 						//任务函数
							(const char *		)"key1_task",		//任务名称
							(uint16_t				)SEND_STK_SIZE,	//任务堆栈大小
							(void *					)NULL,			//传递给任务函数的参数
							(UBaseType_t		)SEND_TASK_PRIO,	//任务优先级
							(TaskHandle_t *	)&send_Task_Handler);	//任务句柄
	//创建信号线接收任务函数
	xTaskCreate((TaskFunction_t	)accept_task, 					//任务函数
				(const char *		)"accpet_task",				//任务名称
				(uint16_t				)ACCEPT_STK_SIZE,		//任务堆栈大小
				(void *					)NULL,					//传递给任务函数的参数
				(UBaseType_t		)ACCEPT_TASK_PRIO,			//任务优先级
				(TaskHandle_t *	)&accept_Task_Handler);			//任务句柄
				
	printf("按下KEY1 获取资源 或 KEY2 释放资源!\r\n");
					
	vTaskDelete(StartTask_Handler);			//删除开始任务
	taskEXIT_CRITICAL();					//退出临界区
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

void send_task(void *pvParameters)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	while (1)
	{
		if( Key_Scan(KEY1_PORT,KEY1_PIN) == KEY_ON )
		{/* K1 被按下 */
			xReturn = xSemaphoreTake(CountingSem_Handle,	//信号量句柄
									500);		//等待时间,时钟节拍数
			if(xReturn == pdTRUE)
				printf("通过K1获取成功！\r\n");
			else if(xReturn == errQUEUE_EMPTY)
				printf("通过K1获取资源失败，资源已经被消耗完！\r\n");
		}
		vTaskDelay(20);
	}
}

void accept_task(void *pvParameters)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	while(1)
	{
		if( Key_Scan(KEY2_PORT,KEY2_PIN) == KEY_ON )
		{/* K1 被按下 */
			xReturn = xSemaphoreGive(CountingSem_Handle);
			if(xReturn == pdTRUE)
				printf("资源通过K2释放成功！\r\n");
			else if(xReturn == errQUEUE_EMPTY)
				printf("资源通过K2释放失败，资源已经完全释放了！\r\n");
		}
		vTaskDelay(20);
	}
	
}
