#include "stm32f10x.h"
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
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
#define KEY1_TASK_PRIO 3
//任务堆栈大小
#define KEY1_STK_SIZE 50
//任务句柄
TaskHandle_t key1_Task_Handler;
//任务函数
void key1_task(void *pvParameters);

//任务优先级
#define KEY2_TASK_PRIO 3
//任务堆栈大小
#define KEY2_STK_SIZE 50
//任务句柄
TaskHandle_t key2_Task_Handler;
//任务函数
void key2_task(void *pvParameters);

//任务优先级
#define MSG_TASK_PRIO 4
//任务堆栈大小
#define MSG_STK_SIZE 50
//任务句柄
TaskHandle_t MSG_Task_Handler;
//任务函数
void msg_task(void *pvParameters);

//消息队列的长度与消息大小(字节)
#define QUEUE_LEN  4
#define QUEUE_SIZE 4

QueueHandle_t Test_Queue = NULL;

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
	KEY_GPIO_Config();							//KEY1,KEY2初始化配置
	USART_Config();									//串口初始化配置
	
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
		
		/* 创建消息队列（Test_Queue） */
		Test_Queue = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* 消息队列的长度 */
															(UBaseType_t ) QUEUE_SIZE);/* 消息的大小 */
		if(NULL != Test_Queue)
		printf("创建Test_Queue消息队列成功!\r\n");
		
		//创建LED1任务函数
		xTaskCreate((TaskFunction_t	)led1_task, 					//任务函数
								(const char *		)"led1_task",					//任务名称
								(uint16_t				)LED1_STK_SIZE,				//任务堆栈大小
								(void *					)NULL,								//传递给任务函数的参数
								(UBaseType_t		)START_TASK_PRIO,			//任务优先级
								(TaskHandle_t *	)&led1Task_Handler);	//任务句柄
		//创建KEY1任务函数
		xTaskCreate((TaskFunction_t	)key1_task, 					//任务函数
								(const char *		)"key1_task",					//任务名称
								(uint16_t				)KEY1_STK_SIZE,				//任务堆栈大小
								(void *					)NULL,								//传递给任务函数的参数
								(UBaseType_t		)KEY1_TASK_PRIO,			//任务优先级
								(TaskHandle_t *	)&key1_Task_Handler);	//任务句柄
		//创建KEY2任务函数
		xTaskCreate((TaskFunction_t	)key2_task, 					//任务函数
					(const char *		)"key2_task",					//任务名称
					(uint16_t				)KEY2_STK_SIZE,				//任务堆栈大小
					(void *					)NULL,								//传递给任务函数的参数
					(UBaseType_t		)KEY2_TASK_PRIO,			//任务优先级
					(TaskHandle_t *	)&key2_Task_Handler);	//任务句柄
		//创建KEY2任务函数
		xTaskCreate((TaskFunction_t	)msg_task, 					//任务函数
					(const char *		)"msg_task",					//任务名称
					(uint16_t				)MSG_STK_SIZE,				//任务堆栈大小
					(void *					)NULL,								//传递给任务函数的参数
					(UBaseType_t		)MSG_TASK_PRIO,			//任务优先级
					(TaskHandle_t *	)&MSG_Task_Handler);	//任务句柄	
		
								
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

void key1_task(void *pvParameters)
{
	  BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
		uint32_t send_data = 1;
		while (1)
		{
				if( Key_Scan(KEY1_PORT,KEY1_PIN) == KEY_ON )
				{/* K1 被按下 */
						printf("发送消息send_data1！\n");
						xReturn = xQueueSend( Test_Queue, /* 消息队列的句柄 */
																	&send_data,/* 发送的消息内容 */
																	0 );        /* 等待时间 0 */
						if(pdPASS == xReturn)
								printf("消息send_data1发送成功！\n\n");
				}
				vTaskDelay(20);/* 延时20个tick */
		}
}

void key2_task(void *pvParameters)
{
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
	uint32_t send_data = 2;
	
	while (1)
	{
		if( Key_Scan(KEY2_PORT,KEY2_PIN) == KEY_ON )
		{/* K2 被按下 */
		printf("发送消息send_data2！\n");
		xReturn = xQueueSend(	Test_Queue, /* 消息队列的句柄 */
								&send_data,/* 发送的消息内容 */
								0 );        /* 等待时间 0 */
		if(pdPASS == xReturn)
			printf("消息send_data2发送成功！\n\n");
		}
		vTaskDelay(20);/* 延时20个tick */
	}
}

void msg_task(void *pvParameters)
{
	BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdTRUE */
	uint32_t r_queue;	/* 定义一个接收消息的变量 */
	while (1)
	{
			xReturn = xQueueReceive( Test_Queue,    /* 消息队列的句柄 */
									 &r_queue,      /* 发送的消息内容 */
									 portMAX_DELAY); /* 等待时间 一直等 */
			if(pdTRUE == xReturn)
			{
				printf("本次接收到的数据:%d\n\n",r_queue);
				if(r_queue == 1)
				{
					GPIO_ResetBits(GPIOB,R_LED);
					vTaskDelay(2000);
					GPIO_SetBits(GPIOB,R_LED);
				}
				if(r_queue == 2)
				{
					GPIO_ResetBits(GPIOB,G_LED);
					vTaskDelay(2000);
					GPIO_SetBits(GPIOB,G_LED);
				}
			}
			else
				printf("数据接收出错,错误代码0x%lx\n",xReturn);
	}
}


