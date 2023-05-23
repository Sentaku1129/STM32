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
#define MEM_TASK_PRIO 4
//任务堆栈大小
#define MEM_STK_SIZE 50
//任务句柄
TaskHandle_t Mem_Task_Handler;
//任务函数
void mem_task(void *pvParameters);

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

	//创建LED1任务函数
	xTaskCreate((TaskFunction_t	)led1_task, 						//任务函数
				(const char *		)"led1_task",		//任务名称
				(uint16_t				)LED1_STK_SIZE,	//任务堆栈大小
				(void *					)NULL,			//传递给任务函数的参数
				(UBaseType_t		)START_TASK_PRIO,	//任务优先级
				(TaskHandle_t *	)&led1Task_Handler);	//任务句柄
	//创建内存读写任务函数
	xTaskCreate((TaskFunction_t	)mem_task, 			//任务函数
				(const char *		)"mem_task",		//任务名称
				(uint16_t				)MEM_STK_SIZE,	//任务堆栈大小
				(void *					)NULL,			//传递给任务函数的参数
				(UBaseType_t		)MEM_TASK_PRIO,	//任务优先级
				(TaskHandle_t *	)&Mem_Task_Handler);	//任务句柄

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

void mem_task(void *pvParameters)
{
	uint8_t *test_ptr = NULL;
	uint32_t g_memsize;
	while (1)
	{
		if(Key_Scan(KEY1_PORT,KEY1_PIN))
		{
			if(test_ptr == NULL)
			{
				//获取当前内存大小
				g_memsize= xPortGetFreeHeapSize();
				printf("系统当前内存大小 %d ,开始申请内存!\r\n",g_memsize);
				test_ptr = pvPortMalloc(1024);
				printf("内存申请成功，申请到的内存地址为%#x\r\n",(int)test_ptr);
				
				//获取当前内存大小
				g_memsize= xPortGetFreeHeapSize();
				printf("系统当前内存大小 %d !\r\n",g_memsize);
				
				//向申请到的内存中写入当前系统时间节拍
				sprintf((char *)test_ptr,"当前系统TickCount = %d",xTaskGetTickCount());
				printf("写入的数据是:%s\r\n",(char *)test_ptr);
			}
			else
				printf("请先按下KEY2释放内存再申请\r\n");
		}
		if(Key_Scan(KEY2_PORT,KEY2_PIN))
		{
			if(test_ptr != NULL)
			{
				printf("开始释放内存！\r\n");
				vPortFree(test_ptr);		//释放内存
				test_ptr = NULL;
				//获取当前内存大小
				g_memsize= xPortGetFreeHeapSize();
				printf("系统当前内存大小 %d ，内存释放完成!\r\n",g_memsize);
			}
			else
				printf("请先按下KEY1申请内存后再释放\r\n");
		}
		vTaskDelay(20);
	}
}

