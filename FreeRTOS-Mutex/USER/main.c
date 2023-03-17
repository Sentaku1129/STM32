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

//�������ȼ�
#define LOW_TASK_PRIO 3
//�����ջ��С
#define LOW_STK_SIZE 50
//������
TaskHandle_t low_Task_Handler;
//������
void low_task(void *pvParameters);

//�������ȼ�
#define MIDDLE_TASK_PRIO 4
//�����ջ��С
#define MIDDLE_STK_SIZE 50
//������
TaskHandle_t middle_Task_Handler;
//������
void middle_task(void *pvParameters);

//�������ȼ�
#define HIGH_TASK_PRIO 5
//�����ջ��С
#define HIGH_STK_SIZE 50
//������
TaskHandle_t high_Task_Handler;
//������
void high_task(void *pvParameters);

//�����ź������
SemaphoreHandle_t Binary_or_Mutex_Sem_Handle = NULL;


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
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	taskENTER_CRITICAL();								//�����ٽ���

	/* ������ֵ�ź��� */
//	Binary_or_Mutex_Sem_Handle = xSemaphoreCreateBinary();
	/* ���������ź��� */
	Binary_or_Mutex_Sem_Handle = xSemaphoreCreateMutex();
	
	xReturn = xSemaphoreGive(Binary_or_Mutex_Sem_Handle);
	if(xReturn == pdTRUE)
	{
		printf("��ʼ���ͷ��ź����ɹ���\r\n\r\n");
	}
	else if(xReturn == errQUEUE_EMPTY)
	{
		printf("�����ź���ʱ����Ҫ�ͷ��ź����� \r\n\r\n");
	}

	//����LED1������
	xTaskCreate((TaskFunction_t	)led1_task, 			//������
				(const char *		)"led1_task",		//��������
				(uint16_t				)LED1_STK_SIZE,	//�����ջ��С
				(void *					)NULL,			//���ݸ��������Ĳ���
				(UBaseType_t		)START_TASK_PRIO,	//�������ȼ�
				(TaskHandle_t *	)&led1Task_Handler);	//������
	//���������ȼ�������
	xTaskCreate((TaskFunction_t	)low_task, 			//������
				(const char *		)"low_task",		//��������
				(uint16_t				)LOW_STK_SIZE,	//�����ջ��С
				(void *					)NULL,			//���ݸ��������Ĳ���
				(UBaseType_t		)LOW_TASK_PRIO,	//�������ȼ�
				(TaskHandle_t *	)&low_Task_Handler);	//������
	//���������ȼ�������
	xTaskCreate((TaskFunction_t	)middle_task, 				//������
			(const char *		)"middle_task",				//��������
			(uint16_t				)MIDDLE_STK_SIZE,		//�����ջ��С
			(void *					)NULL,					//���ݸ��������Ĳ���
			(UBaseType_t		)MIDDLE_TASK_PRIO,			//�������ȼ�
			(TaskHandle_t *	)&middle_Task_Handler);			//������
			
	//���������ȼ�������
	xTaskCreate((TaskFunction_t	)high_task, 				//������
			(const char *		)"high_task",				//��������
			(uint16_t				)HIGH_STK_SIZE,		//�����ջ��С
			(void *					)NULL,					//���ݸ��������Ĳ���
			(UBaseType_t		)HIGH_TASK_PRIO,			//�������ȼ�
			(TaskHandle_t *	)&high_Task_Handler);			//������
					
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

void low_task(void *pvParameters)
{
	int i;
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	while (1)
	{
		xReturn = xSemaphoreTake(Binary_or_Mutex_Sem_Handle,	//�ź������
								portMAX_DELAY);		//�ȴ�ʱ��,ʱ�ӽ�����
		if(xReturn == pdTRUE)
		{
			printf("�����ȼ��������ڱ�ִ�У�\r\n");
		}
		for(i = 0;i < 2000 ;i++)
		{
			taskYIELD();	//�������
		}
//		vTaskDelay(2000);
		printf("�����ȼ����񼴽��ͷ���Դ��\r\n");
		xReturn = xSemaphoreGive(Binary_or_Mutex_Sem_Handle);
		vTaskDelay(500);
	}
}

void middle_task(void *pvParameters)
{
	while(1)
	{
		printf("�����ȼ��������������У�\r\n");
		vTaskDelay(500);
	}
}

void high_task(void *pvParameters)
{
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	while (1)
	{
		xReturn = xSemaphoreTake(Binary_or_Mutex_Sem_Handle,	//�ź������
								portMAX_DELAY);		//�ȴ�ʱ��,ʱ�ӽ�����
		if(xReturn == pdTRUE)
		{
			printf("�����ȼ��������ڱ�ִ�У�\r\n");
		}
		printf("�����ȼ����񼴽��ͷ���Դ��\r\n");
		xReturn = xSemaphoreGive(Binary_or_Mutex_Sem_Handle);
		vTaskDelay(500);
	}
}
