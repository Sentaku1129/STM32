#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"

static u8 fac_us = 0;
static u16 fac_ms = 0;


//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪAHBʱ�ӣ�������������SYSTICKʱ��Ƶ��ΪAHB/8
//SYSTICK:ϵͳʱ��Ƶ��
void	SysTick_Init(u8 SYSTICK)
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//ѡ���ⲿʱ��	HLCK
	fac_us = SystemCoreClock/10000000;//�����Ƿ�����os,fac_us����Ҫʹ��
	reload = SystemCoreClock/10000000;//ÿ���ӵļ�������	��λΪM
	reload *= 1000000/configTICK_RATE_HZ;//����configTICK_RATE_HZ�趨���ʱ��
																			 //reloadΪ24λ�Ĵ���,���ֵ16777216,��72M��ԼΪ0.233s����
	fac_ms = 1000/configTICK_RATE_HZ;			//����os������ʱ����С��λ
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;		//����SYSTICK�ж�
	SysTick->LOAD = reload;											  //ÿ1/configTICK_RATE_HZ���ж�һ��
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	    //����SYSTICK
	
}

//��ʱnus
//nus:Ҫ��ʱ��us��
//nus:0-204522252(���ֵΪ2^32/fac_us@fac_us = 168)
void delay_us(u32 nus)
{
	u32 ticks;
	u32 told,tnow,tcnt = 0;
	u32 reload = SysTick->LOAD;
	ticks = nus*fac_us;
	told = SysTick->VAL;
	while(1)
	{
		tnow  = SysTick->VAL;
		if(tnow != told)
		{
			if(tnow < told)
				tcnt += told - tnow; //SYSTICK��һ���ݼ��ļ�����
			else
				tcnt += reload-tnow+told;
			told = tnow;
			if(tcnt >= ticks)
				break;
		}
	};
}

//��ʱnms
//nms:Ҫ��ʱ��ms��
//nms:0-65535
void delay_ms(u32 nms)
{
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)		//ϵͳ�Ѿ�����
	{
		if(nms >= fac_ms)
		{
			vTaskDelay(nms/fac_ms);																	//FreeRTOS��ʱ
		}
		nms %= fac_ms;
	}
	delay_us((u32)(nms*10000));
}

//��ʱnms,���������������
//nms:Ҫ��ʱ��ms��
void delay_xms(u32 nms)
{
	u32 i;
	for(i = 0;i<nms;i++)
		delay_us(1000);
}

