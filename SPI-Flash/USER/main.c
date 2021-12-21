#include "stm32f10x.h"
#include "usart.h"
#include "spi_flash.h"
#include <stdio.h>

#define R_LED GPIO_Pin_5
#define G_LED GPIO_Pin_0
#define B_LED GPIO_Pin_1


/* ��ȡ�������ĳ��� */
#define TxBufferSize1   (countof(TxBuffer1) - 1)
#define RxBufferSize1   (countof(TxBuffer1) - 1)
#define countof(a)      (sizeof(a) / sizeof(*(a)))
#define  BufferSize (countof(Tx_Buffer)-1)

#define  FLASH_WriteAddress     0x00000
#define  FLASH_ReadAddress      FLASH_WriteAddress
#define  FLASH_SectorToErase    FLASH_WriteAddress
    

/* ���ͻ�������ʼ�� */
uint8_t Tx_Buffer[] = "19310120410-κ�˺�\r\n \
19310120411-������\r\n \
19310120422-�ط�չ\r\n \
19310120427-��Ԫ��\r\n \
19310120442-��˧\r\n";
uint8_t Rx_Buffer[BufferSize];

void LED_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	//������ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	//���ó�ʼ���ṹ��
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_1|GPIO_Pin_0;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
	//���������ʼ�������������úõĽṹ���Աд���Ĵ�������
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	GPIO_SetBits(GPIOB,R_LED|G_LED|B_LED);
}

void GetFlag(char ch)
{
	if(ch == '1')
	{
		/* ������Ҫд��� SPI FLASH ������FLASHд��ǰҪ�Ȳ��� */
		// �������4K����һ����������������С��λ������
		SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 
		
		/* �����ͻ�����������д��flash�� */
		// ����дһҳ��һҳ�Ĵ�СΪ256���ֽ�
		SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);		
		printf("\r\nд�������Ϊ��\r\n %s", Tx_Buffer);
	}
	else if(ch == '0')
	{
		/* ���ո�д������ݶ������ŵ����ջ������� */
		SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
		printf("\r\n����������Ϊ��\r\n %s", Rx_Buffer);
	}
	else
		printf("������0��1\r\n");

}

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

int main()
{
	__IO uint32_t DeviceID = 0;
	__IO uint32_t FlashID = 0;
  LED_GPIO_Config();            //LED��ʼ������
	USART_Config();								//���ڳ�ʼ��	
	SPI_FLASH_Init();							//SPI_FLASH��ʼ��
	
	DeviceID = SPI_FLASH_ReadDeviceID();
	Delay(200);
		
	/* ��ȡ SPI Flash ID */
	FlashID = SPI_FLASH_ReadID();
	printf("FlashID is 0x%X,Manufacturer Device ID is 0x%X \r\n", FlashID, DeviceID);
	
	while(FlashID == sFLASH_ID)
	{
		GetFlag(getchar());
	}
}
