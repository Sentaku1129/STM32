#include "stm32f10x.h"
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "limits.h"
#include "usart.h"
#include "key.h"

#define R_LED GPIO_Pin_5
#define G_LED GPIO_Pin_0
#define B_LED GPIO_Pin_1

/* 配置通过通知模拟代替消息队列——0、二值信号量——1、计数信号量——2、事件组——3 */
#define Notify_Mode 3

/* 启用模拟消息队列时发送数组——0或字符串——1 */
#if (Notify_Mode == 0)
	#define USER_CHAR 1
#elif (Notify_Mode == 3)
	#define KEY1_EVENT (0x01 << 0)
	#define KEY2_EVENT (0x01 << 1)
#endif



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
#define ACCEPT1_TASK_PRIO 3
//任务堆栈大小
#define ACCEPT1_STK_SIZE 50
//任务句柄
TaskHandle_t Accept1_Task_Handler;
//任务函数
void accept1_task(void *pvParameters);

//任务优先级
#define ACCEPT2_TASK_PRIO 3
//任务堆栈大小
#define ACCEPT2_STK_SIZE 50
//任务句柄
TaskHandle_t Accept2_Task_Handler;
//任务函数
void accept2_task(void *pvParameters);

//任务优先级
#define SEND_TASK_PRIO 4
//任务堆栈大小
#define SEND_STK_SIZE 50
//任务句柄
TaskHandle_t Send_Task_Handler;
//任务函数
void send_task(void *pvParameters);

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
	
	xTaskCreate((TaskFunction_t	)start_task, 						//任务函数
							(const char *		)"start_task",		//任务名称
							(uint16_t				)START_STK_SIZE,//任务堆栈大小
							(void *					)NULL,			//传递给任务函数的参数
							(UBaseType_t		)START_TASK_PRIO,	//任务优先级
							(TaskHandle_t *	)&StartTask_Handler);	//任务句柄
	vTaskStartScheduler(); 			//开始任务调度
}

void start_task(void *pvParameters)
{
		taskENTER_CRITICAL();								//进入临界区
		
		
		//创建LED1任务函数
		xTaskCreate((TaskFunction_t	)led1_task, 			//任务函数
					(const char *		)"led1_task",		//任务名称
					(uint16_t				)LED1_STK_SIZE,	//任务堆栈大小
					(void *					)NULL,			//传递给任务函数的参数
					(UBaseType_t		)START_TASK_PRIO,	//任务优先级
					(TaskHandle_t *	)&led1Task_Handler);	//任务句柄
		//创建accept1任务函数
		xTaskCreate((TaskFunction_t	)accept1_task, 			//任务函数
					(const char *	)"accept1_task",		//任务名称
					(uint16_t		)ACCEPT1_STK_SIZE,		//任务堆栈大小
					(void *			)NULL,					//传递给任务函数的参数
					(UBaseType_t	)ACCEPT1_TASK_PRIO,		//任务优先级
					(TaskHandle_t *	)&Accept1_Task_Handler);//任务句柄
		//创建accept2任务函数
		xTaskCreate((TaskFunction_t	)accept2_task, 			//任务函数
					(const char *	)"accept2_task",		//任务名称
					(uint16_t		)ACCEPT2_STK_SIZE,		//任务堆栈大小
					(void *			)NULL,					//传递给任务函数的参数
					(UBaseType_t	)ACCEPT2_TASK_PRIO,		//任务优先级
					(TaskHandle_t *	)&Accept2_Task_Handler);//任务句柄
		//创建send任务函数
		xTaskCreate((TaskFunction_t	)send_task, 			//任务函数
					(const char *		)"send_task",		//任务名称
					(uint16_t				)SEND_STK_SIZE,	//任务堆栈大小
					(void *					)NULL,			//传递给任务函数的参数
					(UBaseType_t		)SEND_TASK_PRIO,	//任务优先级
					(TaskHandle_t *	)&Send_Task_Handler);	//任务句柄	
		
								
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

void accept1_task(void *pvParameters)
{
#if (Notify_Mode != 1 & Notify_Mode != 2)
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
#elif (Notify_Mode == 2)
	uint32_t take_num = pdTRUE;
#endif
	
#if (Notify_Mode == 0)
	
#if USER_CHAR
	char *r_char;
#else
	uint32_t r_num;
#endif
	
	while (1)
	{
		xReturn = xTaskNotifyWait(0x00,					//进入函数时不清除任务bit
								  ULONG_MAX,			//退出任务时清除所有bit
#if USER_CHAR
								  (uint32_t *)&r_char,	//保存任务通知值
#else
								  &r_num,
#endif
								  portMAX_DELAY);		//任务等待时间
		if(xReturn == pdTRUE)
		{
#if USER_CHAR
			printf("accept1任务通知信息为 %s\r\n",r_char);
#else	
			printf("accept1任务通知信息为 %d\r\n",r_num);
#endif
		}
									
	}
#endif

#if (Notify_Mode == 1 | Notify_Mode == 2)
	while(1)
	{
#if (Notify_Mode ==1)
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		printf("accept1 任务通知获取成功!\r\n");
#elif (Notify_Mode ==2)
		take_num = ulTaskNotifyTake(pdFALSE,portMAX_DELAY);
		if(take_num > 0)
			printf("accept2 的资源释放成功！当前剩余资源：%d\r\n",take_num);
		else
			printf("accept2 的资源已经消耗完！请按下KEY1释放资源\r\n");
		vTaskDelay(1000);
#endif
	}
#endif

#if (Notify_Mode == 3)
	uint32_t r_event = 0; 		//定义一个事件接受变量
	uint32_t last_event = 0;	//定义一个事件保存的变量
	while(1)
	{
		xReturn = xTaskNotifyWait(0x00,				//进入函数时不要清除任务bit
								  ULONG_MAX,		//退出函数时清除所有的bit
								  &r_event,			//保存任务通知值
								  portMAX_DELAY);	//阻塞等待时间
		if(xReturn == pdTRUE)
		{
			last_event |= r_event;
			if(last_event == (KEY1_EVENT | KEY2_EVENT))
			{
				vTaskDelay(20);
				last_event = 0;
				printf("KEY1 和 KEY2 都被按下了！\r\n");
			}
		}
	}
#endif

}

void accept2_task(void *pvParameters)
{
#if (Notify_Mode != 1 & Notify_Mode != 2)
	BaseType_t xReturn = pdPASS;/* 定义一个创建信息返回值，默认为pdPASS */
#elif (Notify_Mode == 2)
	uint32_t take_num = pdTRUE;
#endif

#if (Notify_Mode == 0)
	
#if USER_CHAR
	char *r_char;
#else
	uint32_t r_num;
#endif
	
	while (1)
	{
		xReturn = xTaskNotifyWait(0x00,					//进入函数时不清除任务bit
								  ULONG_MAX,			//退出任务时清除所有bit
#if USER_CHAR
								  (uint32_t *)&r_char,	//保存任务通知值
#else
								  &r_num,
#endif
								  portMAX_DELAY);		//任务等待时间
		if(xReturn == pdTRUE)
		{
#if USER_CHAR
			printf("accept2任务通知信息为 %s\r\n",r_char);
#else	
			printf("accept2任务通知信息为 %d\r\n",r_num);
#endif
		}
									
	}

#endif

#if (Notify_Mode == 1 | Notify_Mode == 2)
	while(1)
	{
#if (Notify_Mode ==1)
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		printf("accept2 任务通知获取成功!\r\n");
#elif (Notify_Mode ==2)
		take_num = ulTaskNotifyTake(pdFALSE,portMAX_DELAY);
		if(take_num > 0)
			printf("accept2 的资源释放成功！当前剩余资源：%d\r\n",take_num--);
		else
			printf("accept2 的资源已经消耗完！请按下KEY1释放资源\r\n");
		vTaskDelay(1000);
#endif
	}
#endif

#if (Notify_Mode == 3)
	uint32_t r_event = 0; 		//定义一个事件接受变量
	while(1)
	{
		xReturn = xTaskNotifyWait(0x00,				//进入函数时不要清除任务bit
								  ULONG_MAX,		//退出函数时清除所有的bit
								  &r_event,			//保存任务通知值
								  portMAX_DELAY);	//阻塞等待时间
		if(xReturn == pdTRUE)
		{
			if(r_event == KEY1_EVENT)
			{
				printf("KEY1 事件接收到了！\r\n");
			}
			if(r_event == KEY2_EVENT)
			{
				printf("KEY2 事件接收到了！\r\n");
			}
		}
	}
#endif
}

void send_task(void *pvParameters)
{
#if (Notify_Mode != 3)
	BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdTRUE */
#endif
#if (Notify_Mode == 0)

#if USER_CHAR
	char test_str1[] = "邮箱消息 test1";
	char test_str2[] = "邮箱消息 test2";
#else
	uint32_t send1 = 1;
	uint32_t send2 = 2;
#endif
	while (1)
	{
		if(Key_Scan(KEY1_PORT,KEY1_PIN))
		{
			xReturn = xTaskNotify(Accept1_Task_Handler,			//接受任务句柄
#if USER_CHAR
								  (uint32_t)&test_str1,			//发送的数据，最大为4字节
#else
								  send1,						//发送的数据，最大为4字节
#endif
								  eSetValueWithoutOverwrite);	//覆盖当前通知
			if(xReturn == pdPASS)
			{
				printf("Accept1_Task_Handler的任务通知发送成功\r\n");
			}
		}
		if(Key_Scan(KEY2_PORT,KEY2_PIN))
		{
			xReturn = xTaskNotify(Accept2_Task_Handler,			//接受任务句柄
#if USER_CHAR
								  (uint32_t)&test_str2,			//发送的数据，最大为4字节
#else
								  send2,						//发送的数据，最大为4字节
#endif
								  eSetValueWithoutOverwrite);	//覆盖当前通知
			if(xReturn == pdPASS)
			{
				printf("Accept2_Task_Handler的任务通知发送成功\r\n");
			}
		}
		vTaskDelay(20);
	}
#endif

#if (Notify_Mode == 1 | Notify_Mode == 2)
	while(1)
	{
		if(Key_Scan(KEY1_PORT,KEY1_PIN))
		{
			xReturn = xTaskNotifyGive(Accept1_Task_Handler);
			if(xReturn == pdTRUE)
			{
				printf("KEY1 已经按下！\r\n");
			}
		}
		if(Key_Scan(KEY2_PORT,KEY2_PIN))
		{
			xReturn = xTaskNotifyGive(Accept2_Task_Handler);
			if(xReturn == pdTRUE)
			{
				printf("KEY2 已经按下！\r\n");
			}
		}
		vTaskDelay(20);
	}
#endif

#if (Notify_Mode == 3)
	
	while(1)
	{
		if(Key_Scan(KEY1_PORT,KEY1_PIN))
		{
			printf("KEY1 已经被按下！\r\n");
			xTaskNotify((TaskHandle_t)Accept1_Task_Handler,		//接收任务通知的句柄
						(uint32_t	 )KEY1_EVENT,				//要触发的事件
						 (eNotifyAction)eSetBits);				//设置任务通知值中的位
			xTaskNotify((TaskHandle_t)Accept2_Task_Handler,		//接收任务通知的句柄
						(uint32_t	 )KEY1_EVENT,				//要触发的事件
						(eNotifyAction)eSetBits);				//设置任务通知值中的位
		}
		if(Key_Scan(KEY2_PORT,KEY2_PIN))
		{
			printf("KEY2 已经被按下！\r\n");
			xTaskNotify((TaskHandle_t)Accept1_Task_Handler,		//接收任务通知的句柄
						(uint32_t	 )KEY2_EVENT,				//要触发的事件
						 (eNotifyAction)eSetBits);				//设置任务通知值中的位
			xTaskNotify((TaskHandle_t)Accept2_Task_Handler,		//接收任务通知的句柄
						(uint32_t	 )KEY2_EVENT,				//要触发的事件
						(eNotifyAction)eSetBits);				//设置任务通知值中的位
		}
		vTaskDelay(20);
	}
#endif
}
