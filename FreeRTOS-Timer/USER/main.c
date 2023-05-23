#include "stm32f10x.h"
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "usart.h"
#include "key.h"
#include "timers.h"

#define R_LED GPIO_Pin_5
#define G_LED GPIO_Pin_0
#define B_LED GPIO_Pin_1

//�������ȼ�
#define START_TASK_PRIO 1
//�����ջ��С
#define START_STK_SIZE 128
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);


//�������ȼ�
#define LED1_TASK_PRIO 2
//�����ջ��С
#define LED1_STK_SIZE 50
//������
TaskHandle_t led1Task_Handler;
//������
void led1_task(void *pvParameters);

//�����ʱ�����
TimerHandle_t Timer1_Handle = NULL;
TimerHandle_t Timer2_Handle = NULL;

void timer1_callback(void *parameter);
void timer2_callback(void *parameter);

int Tick_Count = 0;

void LED_GPIO_Config()
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	/*ѡ��Ҫ���Ƶ�GPIO����*/
	GPIO_InitStructure.GPIO_Pin = R_LED|B_LED|G_LED;
	/*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	/*������������Ϊ50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	/*���ÿ⺯������ʼ��GPIOB*/
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/* �ر�����led��	*/
	GPIO_SetBits(GPIOB, GPIO_Pin_0);
	/* �ر�����led��	*/
	GPIO_SetBits(GPIOB, GPIO_Pin_1);	 
	/* �ر�����led��	*/
	GPIO_SetBits(GPIOB, GPIO_Pin_5);
}

int main()
{
	SysTick_Init(72);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	LED_GPIO_Config();            	//GPIOD��ʼ������
	KEY_GPIO_Config();				//KEY1,KEY2��ʼ������
	USART_Config();					//���ڳ�ʼ������
	
	xTaskCreate((TaskFunction_t	)start_task, 								//������
							(const char *		)"start_task",				//��������
							(uint16_t				)START_STK_SIZE,		//�����ջ��С
							(void *					)NULL,					//���ݸ��������Ĳ���
							(UBaseType_t		)START_TASK_PRIO,			//�������ȼ�
							(TaskHandle_t *	)&StartTask_Handler);			//������
	vTaskStartScheduler(); 	//��ʼ�������
}

void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();								//�����ٽ���
	
	Timer1_Handle = xTimerCreate((const char *	)"AutoReloadTimer",
								 (TickType_t	)1000,						//��ʱ������
								 (UBaseType_t	)pdTRUE,					//����ģʽ
								 (void *		)1,							//Ϊ��ʱ������һ��Ψһ����ID
								 (TimerCallbackFunction_t)timer1_callback);	//�ص�����
								 
	if(Timer1_Handle != NULL)
	{
		xTimerStart(Timer1_Handle,0);
	}
	
	Timer2_Handle = xTimerCreate((const char *	)"OnlyReloadTimer",
								 (TickType_t	)5000,						//��ʱ������
								 (UBaseType_t	)pdFALSE,					//����ģʽ
								 (void *		)2,							//Ϊ��ʱ������һ��Ψһ����ID
								 (TimerCallbackFunction_t)timer2_callback);	//�ص�����
								 
	if(Timer2_Handle != NULL)
	{
		xTimerStart(Timer2_Handle,0);
	}

	//����LED1������
	xTaskCreate((TaskFunction_t	)led1_task, 						//������
							(const char *		)"led1_task",		//��������
							(uint16_t				)LED1_STK_SIZE,	//�����ջ��С
							(void *					)NULL,			//���ݸ��������Ĳ���
							(UBaseType_t		)START_TASK_PRIO,	//�������ȼ�
							(TaskHandle_t *	)&led1Task_Handler);	//������
							
	vTaskDelete(StartTask_Handler);			//ɾ����ʼ����
	taskEXIT_CRITICAL();					//�˳��ٽ���
}

//LED1������
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

void timer1_callback(void *parameter)
{
	TickType_t  Tick_Num;
	
	Tick_Count++;
	
	Tick_Num = xTaskGetTickCount();
	
	if(Tick_Count % 2 == 0)
	{
		GPIO_ResetBits(GPIOB,R_LED);
	}
	else
	{
		GPIO_SetBits(GPIOB,R_LED);
	}
	printf("timer1_callback �������� %d ��\r\n",Tick_Count);
	printf("�δ�ʱ��ֵ: %d\r\n",Tick_Num);
}

void timer2_callback(void *parameter)
{
	TickType_t  Tick_Num;
	
	Tick_Count++;
	
	Tick_Num = xTaskGetTickCount();
	
	GPIO_ResetBits(GPIOB,G_LED);
//	vTaskDelay(500);
	GPIO_SetBits(GPIOB,G_LED);
	printf("timer2_callback �δ�ʱ��ֵ: %d\r\n",Tick_Num);
}
