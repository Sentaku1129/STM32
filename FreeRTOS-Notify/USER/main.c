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

/* ����ͨ��֪ͨģ�������Ϣ���С���0����ֵ�ź�������1�������ź�������2���¼��顪��3 */
#define Notify_Mode 3

/* ����ģ����Ϣ����ʱ�������顪��0���ַ�������1 */
#if (Notify_Mode == 0)
	#define USER_CHAR 1
#elif (Notify_Mode == 3)
	#define KEY1_EVENT (0x01 << 0)
	#define KEY2_EVENT (0x01 << 1)
#endif



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
#define ACCEPT1_TASK_PRIO 3
//�����ջ��С
#define ACCEPT1_STK_SIZE 50
//������
TaskHandle_t Accept1_Task_Handler;
//������
void accept1_task(void *pvParameters);

//�������ȼ�
#define ACCEPT2_TASK_PRIO 3
//�����ջ��С
#define ACCEPT2_STK_SIZE 50
//������
TaskHandle_t Accept2_Task_Handler;
//������
void accept2_task(void *pvParameters);

//�������ȼ�
#define SEND_TASK_PRIO 4
//�����ջ��С
#define SEND_STK_SIZE 50
//������
TaskHandle_t Send_Task_Handler;
//������
void send_task(void *pvParameters);

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
	
	xTaskCreate((TaskFunction_t	)start_task, 						//������
							(const char *		)"start_task",		//��������
							(uint16_t				)START_STK_SIZE,//�����ջ��С
							(void *					)NULL,			//���ݸ��������Ĳ���
							(UBaseType_t		)START_TASK_PRIO,	//�������ȼ�
							(TaskHandle_t *	)&StartTask_Handler);	//������
	vTaskStartScheduler(); 			//��ʼ�������
}

void start_task(void *pvParameters)
{
		taskENTER_CRITICAL();								//�����ٽ���
		
		
		//����LED1������
		xTaskCreate((TaskFunction_t	)led1_task, 			//������
					(const char *		)"led1_task",		//��������
					(uint16_t				)LED1_STK_SIZE,	//�����ջ��С
					(void *					)NULL,			//���ݸ��������Ĳ���
					(UBaseType_t		)START_TASK_PRIO,	//�������ȼ�
					(TaskHandle_t *	)&led1Task_Handler);	//������
		//����accept1������
		xTaskCreate((TaskFunction_t	)accept1_task, 			//������
					(const char *	)"accept1_task",		//��������
					(uint16_t		)ACCEPT1_STK_SIZE,		//�����ջ��С
					(void *			)NULL,					//���ݸ��������Ĳ���
					(UBaseType_t	)ACCEPT1_TASK_PRIO,		//�������ȼ�
					(TaskHandle_t *	)&Accept1_Task_Handler);//������
		//����accept2������
		xTaskCreate((TaskFunction_t	)accept2_task, 			//������
					(const char *	)"accept2_task",		//��������
					(uint16_t		)ACCEPT2_STK_SIZE,		//�����ջ��С
					(void *			)NULL,					//���ݸ��������Ĳ���
					(UBaseType_t	)ACCEPT2_TASK_PRIO,		//�������ȼ�
					(TaskHandle_t *	)&Accept2_Task_Handler);//������
		//����send������
		xTaskCreate((TaskFunction_t	)send_task, 			//������
					(const char *		)"send_task",		//��������
					(uint16_t				)SEND_STK_SIZE,	//�����ջ��С
					(void *					)NULL,			//���ݸ��������Ĳ���
					(UBaseType_t		)SEND_TASK_PRIO,	//�������ȼ�
					(TaskHandle_t *	)&Send_Task_Handler);	//������	
		
								
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

void accept1_task(void *pvParameters)
{
#if (Notify_Mode != 1 & Notify_Mode != 2)
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
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
		xReturn = xTaskNotifyWait(0x00,					//���뺯��ʱ���������bit
								  ULONG_MAX,			//�˳�����ʱ�������bit
#if USER_CHAR
								  (uint32_t *)&r_char,	//��������ֵ֪ͨ
#else
								  &r_num,
#endif
								  portMAX_DELAY);		//����ȴ�ʱ��
		if(xReturn == pdTRUE)
		{
#if USER_CHAR
			printf("accept1����֪ͨ��ϢΪ %s\r\n",r_char);
#else	
			printf("accept1����֪ͨ��ϢΪ %d\r\n",r_num);
#endif
		}
									
	}
#endif

#if (Notify_Mode == 1 | Notify_Mode == 2)
	while(1)
	{
#if (Notify_Mode ==1)
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		printf("accept1 ����֪ͨ��ȡ�ɹ�!\r\n");
#elif (Notify_Mode ==2)
		take_num = ulTaskNotifyTake(pdFALSE,portMAX_DELAY);
		if(take_num > 0)
			printf("accept2 ����Դ�ͷųɹ�����ǰʣ����Դ��%d\r\n",take_num);
		else
			printf("accept2 ����Դ�Ѿ������꣡�밴��KEY1�ͷ���Դ\r\n");
		vTaskDelay(1000);
#endif
	}
#endif

#if (Notify_Mode == 3)
	uint32_t r_event = 0; 		//����һ���¼����ܱ���
	uint32_t last_event = 0;	//����һ���¼�����ı���
	while(1)
	{
		xReturn = xTaskNotifyWait(0x00,				//���뺯��ʱ��Ҫ�������bit
								  ULONG_MAX,		//�˳�����ʱ������е�bit
								  &r_event,			//��������ֵ֪ͨ
								  portMAX_DELAY);	//�����ȴ�ʱ��
		if(xReturn == pdTRUE)
		{
			last_event |= r_event;
			if(last_event == (KEY1_EVENT | KEY2_EVENT))
			{
				vTaskDelay(20);
				last_event = 0;
				printf("KEY1 �� KEY2 ���������ˣ�\r\n");
			}
		}
	}
#endif

}

void accept2_task(void *pvParameters)
{
#if (Notify_Mode != 1 & Notify_Mode != 2)
	BaseType_t xReturn = pdPASS;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */
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
		xReturn = xTaskNotifyWait(0x00,					//���뺯��ʱ���������bit
								  ULONG_MAX,			//�˳�����ʱ�������bit
#if USER_CHAR
								  (uint32_t *)&r_char,	//��������ֵ֪ͨ
#else
								  &r_num,
#endif
								  portMAX_DELAY);		//����ȴ�ʱ��
		if(xReturn == pdTRUE)
		{
#if USER_CHAR
			printf("accept2����֪ͨ��ϢΪ %s\r\n",r_char);
#else	
			printf("accept2����֪ͨ��ϢΪ %d\r\n",r_num);
#endif
		}
									
	}

#endif

#if (Notify_Mode == 1 | Notify_Mode == 2)
	while(1)
	{
#if (Notify_Mode ==1)
		ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
		printf("accept2 ����֪ͨ��ȡ�ɹ�!\r\n");
#elif (Notify_Mode ==2)
		take_num = ulTaskNotifyTake(pdFALSE,portMAX_DELAY);
		if(take_num > 0)
			printf("accept2 ����Դ�ͷųɹ�����ǰʣ����Դ��%d\r\n",take_num--);
		else
			printf("accept2 ����Դ�Ѿ������꣡�밴��KEY1�ͷ���Դ\r\n");
		vTaskDelay(1000);
#endif
	}
#endif

#if (Notify_Mode == 3)
	uint32_t r_event = 0; 		//����һ���¼����ܱ���
	while(1)
	{
		xReturn = xTaskNotifyWait(0x00,				//���뺯��ʱ��Ҫ�������bit
								  ULONG_MAX,		//�˳�����ʱ������е�bit
								  &r_event,			//��������ֵ֪ͨ
								  portMAX_DELAY);	//�����ȴ�ʱ��
		if(xReturn == pdTRUE)
		{
			if(r_event == KEY1_EVENT)
			{
				printf("KEY1 �¼����յ��ˣ�\r\n");
			}
			if(r_event == KEY2_EVENT)
			{
				printf("KEY2 �¼����յ��ˣ�\r\n");
			}
		}
	}
#endif
}

void send_task(void *pvParameters)
{
#if (Notify_Mode != 3)
	BaseType_t xReturn = pdTRUE;/* ����һ��������Ϣ����ֵ��Ĭ��ΪpdTRUE */
#endif
#if (Notify_Mode == 0)

#if USER_CHAR
	char test_str1[] = "������Ϣ test1";
	char test_str2[] = "������Ϣ test2";
#else
	uint32_t send1 = 1;
	uint32_t send2 = 2;
#endif
	while (1)
	{
		if(Key_Scan(KEY1_PORT,KEY1_PIN))
		{
			xReturn = xTaskNotify(Accept1_Task_Handler,			//����������
#if USER_CHAR
								  (uint32_t)&test_str1,			//���͵����ݣ����Ϊ4�ֽ�
#else
								  send1,						//���͵����ݣ����Ϊ4�ֽ�
#endif
								  eSetValueWithoutOverwrite);	//���ǵ�ǰ֪ͨ
			if(xReturn == pdPASS)
			{
				printf("Accept1_Task_Handler������֪ͨ���ͳɹ�\r\n");
			}
		}
		if(Key_Scan(KEY2_PORT,KEY2_PIN))
		{
			xReturn = xTaskNotify(Accept2_Task_Handler,			//����������
#if USER_CHAR
								  (uint32_t)&test_str2,			//���͵����ݣ����Ϊ4�ֽ�
#else
								  send2,						//���͵����ݣ����Ϊ4�ֽ�
#endif
								  eSetValueWithoutOverwrite);	//���ǵ�ǰ֪ͨ
			if(xReturn == pdPASS)
			{
				printf("Accept2_Task_Handler������֪ͨ���ͳɹ�\r\n");
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
				printf("KEY1 �Ѿ����£�\r\n");
			}
		}
		if(Key_Scan(KEY2_PORT,KEY2_PIN))
		{
			xReturn = xTaskNotifyGive(Accept2_Task_Handler);
			if(xReturn == pdTRUE)
			{
				printf("KEY2 �Ѿ����£�\r\n");
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
			printf("KEY1 �Ѿ������£�\r\n");
			xTaskNotify((TaskHandle_t)Accept1_Task_Handler,		//��������֪ͨ�ľ��
						(uint32_t	 )KEY1_EVENT,				//Ҫ�������¼�
						 (eNotifyAction)eSetBits);				//��������ֵ֪ͨ�е�λ
			xTaskNotify((TaskHandle_t)Accept2_Task_Handler,		//��������֪ͨ�ľ��
						(uint32_t	 )KEY1_EVENT,				//Ҫ�������¼�
						(eNotifyAction)eSetBits);				//��������ֵ֪ͨ�е�λ
		}
		if(Key_Scan(KEY2_PORT,KEY2_PIN))
		{
			printf("KEY2 �Ѿ������£�\r\n");
			xTaskNotify((TaskHandle_t)Accept1_Task_Handler,		//��������֪ͨ�ľ��
						(uint32_t	 )KEY2_EVENT,				//Ҫ�������¼�
						 (eNotifyAction)eSetBits);				//��������ֵ֪ͨ�е�λ
			xTaskNotify((TaskHandle_t)Accept2_Task_Handler,		//��������֪ͨ�ľ��
						(uint32_t	 )KEY2_EVENT,				//Ҫ�������¼�
						(eNotifyAction)eSetBits);				//��������ֵ֪ͨ�е�λ
		}
		vTaskDelay(20);
	}
#endif
}
