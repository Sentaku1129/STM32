#include "stm32f10x.h"
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
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
#define LED2_TASK_PRIO 3
//任务堆栈大小
#define LED2_STK_SIZE 50
//任务句柄
TaskHandle_t led2_Task_Handler;
//任务函数
void led2_task(void *pvParameters);

//任务优先级
#define KEY_TASK_PRIO 4
//任务堆栈大小
#define KEY_STK_SIZE 50
//任务句柄
TaskHandle_t key_Task_Handler;
//任务函数
void key_task(void *pvParameters);

//事件句柄
EventGroupHandle_t EVENT_Handle = NULL;

//按键事件标志位
#define KEY1_EVENT 	(0x01 << 0)
#define KEY2_EVENT	(0x01 << 1)


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
	
	EVENT_Handle = xEventGroupCreate();
	if(EVENT_Handle != NULL)
	{
		printf("事件组创建成功\r\n");
	}
	else
	{
		printf("事件组创建失败\r\n");
	}

	//创建LED1任务函数
	xTaskCreate((TaskFunction_t	)led1_task, 						//任务函数
							(const char *		)"led1_task",		//任务名称
							(uint16_t				)LED1_STK_SIZE,	//任务堆栈大小
							(void *					)NULL,			//传递给任务函数的参数
							(UBaseType_t		)START_TASK_PRIO,	//任务优先级
							(TaskHandle_t *	)&led1Task_Handler);	//任务句柄
	//创建LED2任务函数
	xTaskCreate((TaskFunction_t		)led2_task, 			//任务函数
				(const char *		)"led2_task",			//任务名称
				(uint16_t			)LED2_STK_SIZE,			//任务堆栈大小
				(void *				)NULL,					//传递给任务函数的参数
				(UBaseType_t		)LED2_TASK_PRIO,		//任务优先级
				(TaskHandle_t *		)&led2_Task_Handler);	//任务句柄
	//创建KEY任务函数
	xTaskCreate((TaskFunction_t	)key_task, 			//任务函数
				(const char *	)"key_task",		//任务名称
				(uint16_t		)KEY_STK_SIZE,		//任务堆栈大小
				(void *			)NULL,				//传递给任务函数的参数
				(UBaseType_t	)KEY_TASK_PRIO,		//任务优先级
				(TaskHandle_t *	)&key_Task_Handler);//任务句柄
							
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

void led2_task(void *pvParameters)
{
	EventBits_t r_event;
	while (1)
	{
		r_event = xEventGroupWaitBits(EVENT_Handle, 			//事件对象句柄
									  KEY1_EVENT | KEY2_EVENT,	//接收感兴趣的事件
									  pdTRUE,					//是否清除标志位
									  pdTRUE,					//等待所有的感兴趣事件
									  portMAX_DELAY);			//等待时间
		if(r_event == (KEY1_EVENT | KEY2_EVENT ))
		{
			printf("KEY1 和 KEY2 都已经按下!\r\n");
			GPIO_ResetBits(GPIOB, R_LED);
			vTaskDelay(1000);
			GPIO_SetBits(GPIOB, R_LED);
		}
		else
		{
			printf("事件错误!\r\n");
		}
	}
}

void key_task(void *pvParameters)
{
	while(1)
	{
		if(Key_Scan(KEY1_PORT,KEY1_PIN))
		{
			printf("KEY1 被按下！\r\n");
			xEventGroupSetBits(EVENT_Handle,		//事件组句柄
							  KEY1_EVENT);		//事件标志位
		}
		if(Key_Scan(KEY2_PORT,KEY2_PIN))
		{
			printf("KEY2 被按下！\r\n");
			xEventGroupSetBits(EVENT_Handle,		//事件组句柄
							  KEY2_EVENT);		//事件标志位
		}
		vTaskDelay(20);
	}
	
}
