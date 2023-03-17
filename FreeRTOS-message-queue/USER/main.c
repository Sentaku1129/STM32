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
#define KEY1_TASK_PRIO 3
//�����ջ��С
#define KEY1_STK_SIZE 50
//������
TaskHandle_t key1_Task_Handler;
//������
void key1_task(void *pvParameters);

//�������ȼ�
#define KEY2_TASK_PRIO 3
//�����ջ��С
#define KEY2_STK_SIZE 50
//������
TaskHandle_t key2_Task_Handler;
//������
void key2_task(void *pvParameters);

//�������ȼ�
#define MSG_TASK_PRIO 4
//�����ջ��С
#define MSG_STK_SIZE 50
//������
TaskHandle_t MSG_Task_Handler;
//������
void msg_task(void *pvParameters);

//��Ϣ���еĳ�������Ϣ��С(�ֽ�)
#define QUEUE_LEN  4
#define QUEUE_SIZE 4

QueueHandle_t Test_Queue = NULL;

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
	KEY_GPIO_Config();							//KEY1,KEY2��ʼ������
	USART_Config();									//���ڳ�ʼ������
	
	xTaskCreate((TaskFunction_t	)start_task, 					//������
							(const char *		)"start_task",				//��������
							(uint16_t				)START_STK_SIZE,			//�����ջ��С
							(void *					)NULL,								//���ݸ��������Ĳ���
							(UBaseType_t		)START_TASK_PRIO,			//�������ȼ�
							(TaskHandle_t *	)&StartTask_Handler);	//������
	vTaskStartScheduler(); 	//��ʼ�������
}

void start_task(void *pvParameters)
{
		taskENTER_CRITICAL();								//�����ٽ���
		
		/* ������Ϣ���У�Test_Queue�� */
		Test_Queue = xQueueCreate((UBaseType_t ) QUEUE_LEN,/* ��Ϣ���еĳ��� */
															(UBaseType_t ) QUEUE_SIZE);/* ��Ϣ�Ĵ�С */
		if(NULL != Test_Queue)
		printf("����Test_Queue��Ϣ���гɹ�!\r\n");
		
		//����LED1������
		xTaskCreate((TaskFunction_t	)led1_task, 					//������
								(const char *		)"led1_task",					//��������
								(uint16_t				)LED1_STK_SIZE,				//�����ջ��С
								(void *					)NULL,								//���ݸ��������Ĳ���
								(UBaseType_t		)START_TASK_PRIO,			//�������ȼ�
								(TaskHandle_t *	)&led1Task_Handler);	//������
		//����KEY1������
		xTaskCreate((TaskFunction_t	)key1_task, 					//������
								(const char *		)"key1_task",					//��������
								(uint16_t				)KEY1_STK_SIZE,				//�����ջ��С
								(void *					)NULL,								//���ݸ��������Ĳ���
								(UBaseType_t		)KEY1_TASK_PRIO,			//�������ȼ�
								(TaskHandle_t *	)&key1_Task_Handler);	//������
		//����KEY2������
		xTaskCreate((TaskFunction_t	)key2_task, 					//������
					(const char *		)"key2_task",					//��������
					(uint16_t				)KEY2_STK_SIZE,				//�����ջ��С
					(void *					)NULL,								//���ݸ��������Ĳ���
					(UBaseType_t		)KEY2_TASK_PRIO,			//�������ȼ�
					(TaskHandle_t *	)&key2_Task_Handler);	//������
		//����KEY2������
		xTaskCreate((TaskFunction_t	)msg_task, 					//������
					(const char *		)"msg_task",					//��������
					(uint16_t				)MSG_STK_SIZE,				//�����ջ��С
					(void *					)NULL,								//���ݸ��������Ĳ���
					(UBaseType_t		)MSG_TASK_PRIO,			//�������ȼ�
					(TaskHandle_t *	)&MSG_Task_Handler);	//������	
		
								
		vTaskDelete(StartTask_Handler);			//ɾ����ʼ����
		taskEXIT_CRITICAL();								//�˳��ٽ���
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

void key1_task(void *pvParameters)
{
	  BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
		uint32_t send_data = 1;
		while (1)
		{
				if( Key_Scan(KEY1_PORT,KEY1_PIN) == KEY_ON )
				{/* K1 ������ */
						printf("������Ϣsend_data1��\n");
						xReturn = xQueueSend( Test_Queue, /* ��Ϣ���еľ�� */
																	&send_data,/* ���͵���Ϣ���� */
																	0 );        /* �ȴ�ʱ�� 0 */
						if(pdPASS == xReturn)
								printf("��Ϣsend_data1���ͳɹ���\n\n");
				}
				vTaskDelay(20);/* ��ʱ20��tick */
		}
}

void key2_task(void *pvParameters)
{
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
	uint32_t send_data = 2;
	
	while (1)
	{
		if( Key_Scan(KEY2_PORT,KEY2_PIN) == KEY_ON )
		{/* K2 ������ */
		printf("������Ϣsend_data2��\n");
		xReturn = xQueueSend(	Test_Queue, /* ��Ϣ���еľ�� */
								&send_data,/* ���͵���Ϣ���� */
								0 );        /* �ȴ�ʱ�� 0 */
		if(pdPASS == xReturn)
			printf("��Ϣsend_data2���ͳɹ���\n\n");
		}
		vTaskDelay(20);/* ��ʱ20��tick */
	}
}

void msg_task(void *pvParameters)
{
	BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE */
	uint32_t r_queue;	/* ����һ��������Ϣ�ı��� */
	while (1)
	{
			xReturn = xQueueReceive( Test_Queue,    /* ��Ϣ���еľ�� */
									 &r_queue,      /* ���͵���Ϣ���� */
									 portMAX_DELAY); /* �ȴ�ʱ�� һֱ�� */
			if(pdTRUE == xReturn)
			{
				printf("���ν��յ�������:%d\n\n",r_queue);
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
				printf("���ݽ��ճ���,�������0x%lx\n",xReturn);
	}
}


