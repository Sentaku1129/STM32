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
#define MEM_TASK_PRIO 4
//�����ջ��С
#define MEM_STK_SIZE 50
//������
TaskHandle_t Mem_Task_Handler;
//������
void mem_task(void *pvParameters);

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

	//����LED1������
	xTaskCreate((TaskFunction_t	)led1_task, 						//������
				(const char *		)"led1_task",		//��������
				(uint16_t				)LED1_STK_SIZE,	//�����ջ��С
				(void *					)NULL,			//���ݸ��������Ĳ���
				(UBaseType_t		)START_TASK_PRIO,	//�������ȼ�
				(TaskHandle_t *	)&led1Task_Handler);	//������
	//�����ڴ��д������
	xTaskCreate((TaskFunction_t	)mem_task, 			//������
				(const char *		)"mem_task",		//��������
				(uint16_t				)MEM_STK_SIZE,	//�����ջ��С
				(void *					)NULL,			//���ݸ��������Ĳ���
				(UBaseType_t		)MEM_TASK_PRIO,	//�������ȼ�
				(TaskHandle_t *	)&Mem_Task_Handler);	//������

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
				//��ȡ��ǰ�ڴ��С
				g_memsize= xPortGetFreeHeapSize();
				printf("ϵͳ��ǰ�ڴ��С %d ,��ʼ�����ڴ�!\r\n",g_memsize);
				test_ptr = pvPortMalloc(1024);
				printf("�ڴ�����ɹ������뵽���ڴ��ַΪ%#x\r\n",(int)test_ptr);
				
				//��ȡ��ǰ�ڴ��С
				g_memsize= xPortGetFreeHeapSize();
				printf("ϵͳ��ǰ�ڴ��С %d !\r\n",g_memsize);
				
				//�����뵽���ڴ���д�뵱ǰϵͳʱ�����
				sprintf((char *)test_ptr,"��ǰϵͳTickCount = %d",xTaskGetTickCount());
				printf("д���������:%s\r\n",(char *)test_ptr);
			}
			else
				printf("���Ȱ���KEY2�ͷ��ڴ�������\r\n");
		}
		if(Key_Scan(KEY2_PORT,KEY2_PIN))
		{
			if(test_ptr != NULL)
			{
				printf("��ʼ�ͷ��ڴ棡\r\n");
				vPortFree(test_ptr);		//�ͷ��ڴ�
				test_ptr = NULL;
				//��ȡ��ǰ�ڴ��С
				g_memsize= xPortGetFreeHeapSize();
				printf("ϵͳ��ǰ�ڴ��С %d ���ڴ��ͷ����!\r\n",g_memsize);
			}
			else
				printf("���Ȱ���KEY1�����ڴ�����ͷ�\r\n");
		}
		vTaskDelay(20);
	}
}

