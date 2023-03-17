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
#define LOW_TASK_PRIO 3
//任务堆栈大小
#define LOW_STK_SIZE 50
//任务句柄
TaskHandle_t low_Task_Handler;
//任务函数
void low_task(void *pvParameters);

//任务优先级
#define MIDDLE_TASK_PRIO 4
//任务堆栈大小
#define MIDDLE_STK_SIZE 50
//任务句柄
TaskHandle_t middle_Task_Handler;
//任务函数
void middle_task(void *pvParameters);

//任务优先级
#define HIGH_TASK_PRIO 5
//任务堆栈大小
#define HIGH_STK_SIZE 50
//任务句柄
TaskHandle_t high_Task_Handler;
//任务函数
void high_task(void *pvParameters);

//计数信号量句柄
SemaphoreHandle_t Binary_or_Mutex_Sem_Handle = NULL;


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
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	taskENTER_CRITICAL();								//进入临界区

	/* 创建二值信号量 */
//	Binary_or_Mutex_Sem_Handle = xSemaphoreCreateBinary();
	/* 创建互斥信号量 */
	Binary_or_Mutex_Sem_Handle = xSemaphoreCreateMutex();
	
	xReturn = xSemaphoreGive(Binary_or_Mutex_Sem_Handle);
	if(xReturn == pdTRUE)
	{
		printf("初始化释放信号量成功！\r\n\r\n");
	}
	else if(xReturn == errQUEUE_EMPTY)
	{
		printf("互斥信号量时不需要释放信号量！ \r\n\r\n");
	}

	//创建LED1任务函数
	xTaskCreate((TaskFunction_t	)led1_task, 			//任务函数
				(const char *		)"led1_task",		//任务名称
				(uint16_t				)LED1_STK_SIZE,	//任务堆栈大小
				(void *					)NULL,			//传递给任务函数的参数
				(UBaseType_t		)START_TASK_PRIO,	//任务优先级
				(TaskHandle_t *	)&led1Task_Handler);	//任务句柄
	//创建低优先级任务函数
	xTaskCreate((TaskFunction_t	)low_task, 			//任务函数
				(const char *		)"low_task",		//任务名称
				(uint16_t				)LOW_STK_SIZE,	//任务堆栈大小
				(void *					)NULL,			//传递给任务函数的参数
				(UBaseType_t		)LOW_TASK_PRIO,	//任务优先级
				(TaskHandle_t *	)&low_Task_Handler);	//任务句柄
	//创建中优先级任务函数
	xTaskCreate((TaskFunction_t	)middle_task, 				//任务函数
			(const char *		)"middle_task",				//任务名称
			(uint16_t				)MIDDLE_STK_SIZE,		//任务堆栈大小
			(void *					)NULL,					//传递给任务函数的参数
			(UBaseType_t		)MIDDLE_TASK_PRIO,			//任务优先级
			(TaskHandle_t *	)&middle_Task_Handler);			//任务句柄
			
	//创建中优先级任务函数
	xTaskCreate((TaskFunction_t	)high_task, 				//任务函数
			(const char *		)"high_task",				//任务名称
			(uint16_t				)HIGH_STK_SIZE,		//任务堆栈大小
			(void *					)NULL,					//传递给任务函数的参数
			(UBaseType_t		)HIGH_TASK_PRIO,			//任务优先级
			(TaskHandle_t *	)&high_Task_Handler);			//任务句柄
					
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

void low_task(void *pvParameters)
{
	int i;
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	while (1)
	{
		xReturn = xSemaphoreTake(Binary_or_Mutex_Sem_Handle,	//信号量句柄
								portMAX_DELAY);		//等待时间,时钟节拍数
		if(xReturn == pdTRUE)
		{
			printf("低优先级任务正在被执行！\r\n");
		}
		for(i = 0;i < 2000 ;i++)
		{
			taskYIELD();	//任务调度
		}
//		vTaskDelay(2000);
		printf("低优先级任务即将释放资源！\r\n");
		xReturn = xSemaphoreGive(Binary_or_Mutex_Sem_Handle);
		vTaskDelay(500);
	}
}

void middle_task(void *pvParameters)
{
	while(1)
	{
		printf("中优先级任务正在运行中！\r\n");
		vTaskDelay(500);
	}
}

void high_task(void *pvParameters)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	while (1)
	{
		xReturn = xSemaphoreTake(Binary_or_Mutex_Sem_Handle,	//信号量句柄
								portMAX_DELAY);		//等待时间,时钟节拍数
		if(xReturn == pdTRUE)
		{
			printf("高优先级任务正在被执行！\r\n");
		}
		printf("高优先级任务即将释放资源！\r\n");
		xReturn = xSemaphoreGive(Binary_or_Mutex_Sem_Handle);
		vTaskDelay(500);
	}
}
