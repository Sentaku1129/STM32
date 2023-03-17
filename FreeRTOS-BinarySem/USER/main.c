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
#define SEND_TASK_PRIO 4
//�����ջ��С
#define SEND_STK_SIZE 50
//������
TaskHandle_t send_Task_Handler;
//������
void send_task(void *pvParameters);

//�������ȼ�
#define ACCEPT_TASK_PRIO 3
//�����ջ��С
#define ACCEPT_STK_SIZE 50
//������
TaskHandle_t accept_Task_Handler;
//������
void accept_task(void *pvParameters);

//��ֵ�ź������
SemaphoreHandle_t BinarySem_Handle = NULL;


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

	/* ������ֵ�ź��� */
	BinarySem_Handle = xSemaphoreCreateBinary();

	//����LED1������
	xTaskCreate((TaskFunction_t	)led1_task, 						//������
							(const char *		)"led1_task",		//��������
							(uint16_t				)LED1_STK_SIZE,	//�����ջ��С
							(void *					)NULL,			//���ݸ��������Ĳ���
							(UBaseType_t		)START_TASK_PRIO,	//�������ȼ�
							(TaskHandle_t *	)&led1Task_Handler);	//������
	//�����ź��������ͷ�������
	xTaskCreate((TaskFunction_t	)send_task, 						//������
							(const char *		)"key1_task",		//��������
							(uint16_t				)SEND_STK_SIZE,	//�����ջ��С
							(void *					)NULL,			//���ݸ��������Ĳ���
							(UBaseType_t		)SEND_TASK_PRIO,	//�������ȼ�
							(TaskHandle_t *	)&send_Task_Handler);	//������
	//�����ź��߽���������
	xTaskCreate((TaskFunction_t	)accept_task, 					//������
				(const char *		)"accpet_task",				//��������
				(uint16_t				)ACCEPT_STK_SIZE,		//�����ջ��С
				(void *					)NULL,					//���ݸ��������Ĳ���
				(UBaseType_t		)ACCEPT_TASK_PRIO,			//�������ȼ�
				(TaskHandle_t *	)&accept_Task_Handler);			//������
				
	printf("����KEY1 �� KEY2 �ͷ��ź���!\r\n");
	printf("�ź�������ȡ֮���ɫ����0.2s\r\n");
					
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

void send_task(void *pvParameters)
{
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	while (1)
	{
		if( Key_Scan(KEY1_PORT,KEY1_PIN) == KEY_ON )
		{/* K1 ������ */
			xReturn = xSemaphoreGive(BinarySem_Handle);
			if(xReturn == pdTRUE)
				printf("�ź���ͨ��K1�ͷųɹ���\r\n");
			else if(xReturn == errQUEUE_EMPTY)
				printf("�ź���ͨ��K1�ͷ�ʧ�ܣ�\r\n");
		}
		if( Key_Scan(KEY2_PORT,KEY2_PIN) == KEY_ON )
		{/* K1 ������ */
			xReturn = xSemaphoreGive(BinarySem_Handle);
			if(xReturn == pdTRUE)
				printf("�ź���ͨ��K2�ͷųɹ���\r\n");
			else if(xReturn == errQUEUE_EMPTY)
				printf("�ź���ͨ��K2�ͷ�ʧ�ܣ�\r\n");
		}
		vTaskDelay(20);
	}
}

void accept_task(void *pvParameters)
{
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	while(1)
	{
		xReturn = xSemaphoreTake(BinarySem_Handle, 	//��ֵ�ź������
								portMAX_DELAY);		//�ȴ�ʱ��
		if(xReturn == pdTRUE)
		{
			printf("��ֵ�ź�����ȡ�ɹ�\r\n");
			GPIO_ResetBits(GPIOB, R_LED);
			vTaskDelay(200);
			GPIO_SetBits(GPIOB, R_LED);
		}
	}
	
}
