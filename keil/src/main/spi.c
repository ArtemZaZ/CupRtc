#include "spi.h"

extern volatile int16_t OUT_Buffer[2][160];
extern volatile int16_t *outBuffer;
extern volatile uint8_t Start_Decoding;

void spi_init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	//Настраиваем SPI2
	SPI_InitTypeDef spi2;
	spi2.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi2.SPI_Mode = SPI_Mode_Master;
	spi2.SPI_DataSize = SPI_DataSize_8b;
	spi2.SPI_CPOL = SPI_CPOL_Low;
	spi2.SPI_CPHA = SPI_CPHA_1Edge;
	spi2.SPI_NSS = SPI_NSS_Soft;
	spi2.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	spi2.SPI_FirstBit = SPI_FirstBit_MSB;
	spi2.SPI_CRCPolynomial = 10;
	SPI_Init(SPI2, &spi2);
	SPI_Cmd(SPI2, ENABLE);

	//Настраиваем MOSI и CLK и MISO 
	GPIO_InitTypeDef MosiAndCLK;
	MosiAndCLK.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15 | GPIO_Pin_14;
	MosiAndCLK.GPIO_Mode = GPIO_Mode_AF_PP;
	MosiAndCLK.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &MosiAndCLK);
	
	//Настраиваем CS_2
	GPIO_InitTypeDef CS2;
	CS2.GPIO_Mode = GPIO_Mode_Out_PP;
	CS2.GPIO_Pin = GPIO_Pin_9;
	CS2.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &CS2);
	GPIO_SetBits(GPIOA, GPIO_Pin_9);
	//Настраиваем CS_0
	GPIO_InitTypeDef CS0;
	CS0.GPIO_Mode = GPIO_Mode_Out_PP;
	CS0.GPIO_Pin = GPIO_Pin_12;
	CS0.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &CS0);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	
	//Настраиваем TIM2	
	TIM_TimeBaseInitTypeDef Tim2;
	Tim2.TIM_Prescaler = 4500; //необходима частота 8кгц
	Tim2.TIM_ClockDivision = TIM_CKD_DIV1;
	Tim2.TIM_Period = 1;
	Tim2.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &Tim2);
	NVIC_SetPriority(TIM2_IRQn, 0);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//TIM_Cmd(TIM2, ENABLE);
}

extern uint8_t interrupt_flag;

void TIM2_IRQHandler(){
	NVIC_DisableIRQ(TIM2_IRQn);
	
	//UBaseType_t uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	//Читаем из текущего буфера значение;
	//понижаем его разрядность и прибавляем 512, поскольку
	//в буфере число со знаком;
	uint16_t tmp;
	tmp = (uint16_t)(((*outBuffer>>5)) + 0x1500);
	GPIO_ResetBits(GPIOA, GPIO_Pin_9);
	SPI_I2S_SendData(SPI2, (tmp | 0x3000) >> 8);
	SPI_I2S_SendData(SPI2, (tmp | 0x3000));
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
	GPIO_SetBits(GPIOA, GPIO_Pin_9);
	//Если дошли до конца буфера, изменяем указатель на другой буфер
	//и начинаем декодировать данные;
	
	if(outBuffer == &OUT_Buffer[1][159])
	{
		outBuffer = OUT_Buffer[0];
		Start_Decoding = 2;
	}
	else if(outBuffer == &OUT_Buffer[0][159])
	{
		outBuffer++;
		Start_Decoding = 1;
	}
	else
	{
		outBuffer++;
	}
	//taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
	NVIC_EnableIRQ(TIM2_IRQn);
}

