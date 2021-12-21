#include "stm32f10x.h"
#include "usart.h"
#include "spi_flash.h"
#include <stdio.h>

#define R_LED GPIO_Pin_5
#define G_LED GPIO_Pin_0
#define B_LED GPIO_Pin_1


/* 获取缓冲区的长度 */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer)-1)

#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
    

/* 发送缓冲区初始化 */
uint8_t Tx_Buffer[] = "19310120410-魏兴湖\r\n \
19310120411-蒋永洪\r\n \
19310120422-秦发展\r\n \
19310120427-游元芳\r\n \
19310120442-雷帅\r\n";
uint8_t Rx_Buffer[BufferSize];

void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//打开外设时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//配置初始化结构体
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_1|GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	//调用外设初始化函数，把配置好的结构体成员写到寄存器里面
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_SetBits(GPIOB,R_LED|G_LED|B_LED);
}

void GetFlag(char ch)
{
	if(ch == '1')
	{
		/* 擦除将要写入的 SPI FLASH 扇区，FLASH写入前要先擦除 */
		// 这里擦除4K，即一个扇区，擦除的最小单位是扇区
		SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 
		
		/* 将发送缓冲区的数据写到flash中 */
		// 这里写一页，一页的大小为256个字节
		SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);		
		printf("\r\n写入的数据为：\r\n %s", Tx_Buffer);
	}
	else if(ch == '0')
	{
		/* 将刚刚写入的数据读出来放到接收缓冲区中 */
		SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
		printf("\r\n读出的数据为：\r\n %s", Rx_Buffer);
	}
	else
		printf("请输入0或1\r\n");

}

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

int main()
{
	__IO uint32_t DeviceID = 0;
	__IO uint32_t FlashID = 0;
  LED_GPIO_Config();            //LED初始化配置
	USART_Config();								//串口初始化	
	SPI_FLASH_Init();							//SPI_FLASH初始化
	
	DeviceID = SPI_FLASH_ReadDeviceID();
	Delay(200);
		
	/* 获取 SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();
	printf("FlashID is 0x%X,Manufacturer Device ID is 0x%X \r\n", FlashID, DeviceID);
	
	while(FlashID == sFLASH_ID)
	{
		GetFlag(getchar());
	}
}
