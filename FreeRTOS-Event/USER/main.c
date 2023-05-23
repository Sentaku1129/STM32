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
#define LED2_TASK_PRIO 3
//�����ջ��С
#define LED2_STK_SIZE 50
//������
TaskHandle_t led2_Task_Handler;
//������
void led2_task(void *pvParameters);

//�������ȼ�
#define KEY_TASK_PRIO 4
//�����ջ��С
#define KEY_STK_SIZE 50
//������
TaskHandle_t key_Task_Handler;
//������
void key_task(void *pvParameters);

//�¼����
EventGroupHandle_t EVENT_Handle = NULL;

//�����¼���־λ
#define KEY1_EVENT 	(0x01 << 0)
#define KEY2_EVENT	(0x01 << 1)


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
	
	EVENT_Handle = xEventGroupCreate();
	if(EVENT_Handle != NULL)
	{
		printf("�¼��鴴���ɹ�\r\n");
	}
	else
	{
		printf("�¼��鴴��ʧ��\r\n");
	}

	//����LED1������
	xTaskCreate((TaskFunction_t	)led1_task, 						//������
							(const char *		)"led1_task",		//��������
							(uint16_t				)LED1_STK_SIZE,	//�����ջ��С
							(void *					)NULL,			//���ݸ��������Ĳ���
							(UBaseType_t		)START_TASK_PRIO,	//�������ȼ�
							(TaskHandle_t *	)&led1Task_Handler);	//������
	//����LED2������
	xTaskCreate((TaskFunction_t		)led2_task, 			//������
				(const char *		)"led2_task",			//��������
				(uint16_t			)LED2_STK_SIZE,			//�����ջ��С
				(void *				)NULL,					//���ݸ��������Ĳ���
				(UBaseType_t		)LED2_TASK_PRIO,		//�������ȼ�
				(TaskHandle_t *		)&led2_Task_Handler);	//������
	//����KEY������
	xTaskCreate((TaskFunction_t	)key_task, 			//������
				(const char *	)"key_task",		//��������
				(uint16_t		)KEY_STK_SIZE,		//�����ջ��С
				(void *			)NULL,				//���ݸ��������Ĳ���
				(UBaseType_t	)KEY_TASK_PRIO,		//�������ȼ�
				(TaskHandle_t *	)&key_Task_Handler);//������
							
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

void led2_task(void *pvParameters)
{
	EventBits_t r_event;
	while (1)
	{
		r_event = xEventGroupWaitBits(EVENT_Handle, 			//�¼�������
									  KEY1_EVENT | KEY2_EVENT,	//���ո���Ȥ���¼�
									  pdTRUE,					//�Ƿ������־λ
									  pdTRUE,					//�ȴ����еĸ���Ȥ�¼�
									  portMAX_DELAY);			//�ȴ�ʱ��
		if(r_event == (KEY1_EVENT | KEY2_EVENT ))
		{
			printf("KEY1 �� KEY2 ���Ѿ�����!\r\n");
			GPIO_ResetBits(GPIOB, R_LED);
			vTaskDelay(1000);
			GPIO_SetBits(GPIOB, R_LED);
		}
		else
		{
			printf("�¼�����!\r\n");
		}
	}
}

void key_task(void *pvParameters)
{
	while(1)
	{
		if(Key_Scan(KEY1_PORT,KEY1_PIN))
		{
			printf("KEY1 �����£�\r\n");
			xEventGroupSetBits(EVENT_Handle,		//�¼�����
							  KEY1_EVENT);		//�¼���־λ
		}
		if(Key_Scan(KEY2_PORT,KEY2_PIN))
		{
			printf("KEY2 �����£�\r\n");
			xEventGroupSetBits(EVENT_Handle,		//�¼�����
							  KEY2_EVENT);		//�¼���־λ
		}
		vTaskDelay(20);
	}
	
}
