#include "stm32f10x.h"
#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usart.h"
#include "key.h"
#include "interrupt.h"
#include <string.h>

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
#define CPU_TASK_PRIO 4
//�����ջ��С
#define CPU_STK_SIZE 50
//������
TaskHandle_t Cpu_Task_Handler;
//������
void cpu_task(void *pvParameters);

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
	
	TIM6_Init(100,72);			//��ʱ0.1ms
	
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

	//����LED1������
	xTaskCreate((TaskFunction_t	)led1_task, 			//������
				(const char *	)"led1_task",			//��������
				(uint16_t		)LED1_STK_SIZE,			//�����ջ��С
				(void *			)NULL,					//���ݸ��������Ĳ���
				(UBaseType_t	)START_TASK_PRIO,		//�������ȼ�
				(TaskHandle_t *	)&led1Task_Handler);	//������
	//�����ж�������
	xTaskCreate((TaskFunction_t	)cpu_task, 			//������
				(const char *	)"cpu_task",			//��������
				(uint16_t		)CPU_STK_SIZE,		//�����ջ��С
				(void *			)NULL,						//���ݸ��������Ĳ���
				(UBaseType_t	)CPU_TASK_PRIO,		//�������ȼ�
				(TaskHandle_t *	)&Cpu_Task_Handler);	//������
					
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

void cpu_task(void *pvParameters)
{
	static uint8_t CPU_RunInfo[400];//������������ʱ����Ϣ
	while (1)
	{
		memset(CPU_RunInfo,0,400);		//��Ϣ��������
		vTaskList((char *)&CPU_RunInfo);	//��ȡ��������ʱ����Ϣ
		
		printf("--------------------------------------------\r\n");
		printf("������     ����״̬ ���ȼ�   ʣ��ջ  ������� \r\n");
		printf("%s",CPU_RunInfo);
		printf("--------------------------------------------\r\n");
		
		memset(CPU_RunInfo,0,400);		//��Ϣ��������
		vTaskGetRunTimeStats((char *)&CPU_RunInfo);
		
		printf("������     ���м���           ������          \r\n");
		printf("%s",CPU_RunInfo);
		printf("---------------------------------------------\r\n");
		vTaskDelay(1000);
		
	}
}
