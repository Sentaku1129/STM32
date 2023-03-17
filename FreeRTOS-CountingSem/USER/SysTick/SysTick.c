#include "SysTick.h"
#include "FreeRTOS.h"
#include "task.h"

static u8 fac_us = 0;
static u16 fac_ms = 0;


//初始化延迟函数
//SYSTICK的时钟固定为AHB时钟，基础例程里面SYSTICK时钟频率为AHB/8
//SYSTICK:系统时钟频率
void	SysTick_Init(u8 SYSTICK)
{
	u32 reload;
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);//选择外部时钟	HLCK
	fac_us = SystemCoreClock/10000000;//不论是否适用os,fac_us都需要使用
	reload = SystemCoreClock/10000000;//每秒钟的计数次数	单位为M
	reload *= 1000000/configTICK_RATE_HZ;//根据configTICK_RATE_HZ设定溢出时间
																			 //reload为24位寄存器,最大值16777216,在72M下约为0.233s左右
	fac_ms = 1000/configTICK_RATE_HZ;			//代表os可以延时的最小单位
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;		//开启SYSTICK中断
	SysTick->LOAD = reload;											  //每1/configTICK_RATE_HZ秒中断一下
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	    //开启SYSTICK
	
}

//延时nus
//nus:要延时的us数
//nus:0-204522252(最大值为2^32/fac_us@fac_us = 168)
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
				tcnt += told - tnow; //SYSTICK是一个递减的计数器
			else
				tcnt += reload-tnow+told;
			told = tnow;
			if(tcnt >= ticks)
				break;
		}
	};
}

//延时nms
//nms:要延时的ms数
//nms:0-65535
void delay_ms(u32 nms)
{
	if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)		//系统已经运行
	{
		if(nms >= fac_ms)
		{
			vTaskDelay(nms/fac_ms);																	//FreeRTOS延时
		}
		nms %= fac_ms;
	}
	delay_us((u32)(nms*10000));
}

//延时nms,不会引起任务调度
//nms:要延时的ms数
void delay_xms(u32 nms)
{
	u32 i;
	for(i = 0;i<nms;i++)
		delay_us(1000);
}

