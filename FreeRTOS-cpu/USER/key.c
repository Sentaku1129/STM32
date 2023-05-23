#include "key.h"

void KEY_GPIO_Config(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		/*���������˿ڵ�ʱ��*/
		RCC_APB2PeriphClockCmd(KEY1_CLK|KEY2_CLK,ENABLE);
		
		//ѡ�񰴼�������
		GPIO_InitStructure.GPIO_Pin = KEY1_PIN; 
		// ���ð���������Ϊ��������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
		//ʹ�ýṹ���ʼ������
		GPIO_Init(KEY1_PORT, &GPIO_InitStructure);
		
		//ѡ�񰴼�������
		GPIO_InitStructure.GPIO_Pin = KEY2_PIN; 
		//���ð���������Ϊ��������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
		//ʹ�ýṹ���ʼ������
		GPIO_Init(KEY2_PORT, &GPIO_InitStructure);	
}

uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	/*����Ƿ��а������� */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		/*�ȴ������ͷ� */
		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
		return 	KEY_ON;	 
	}
	else
		return KEY_OFF;
}
