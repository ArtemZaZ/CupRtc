#ifndef SPX_H
#define SPX_H

/*
*Библиотека для работы speex-а и динамика
*ВНИМАНИЕ
*вся библиотека подстроена под работу с системой freertos
*/

#include <speex/speex.h>
#include <stdint.h>
#include "EEPROM.h"
#include "Prj_config.h"

#define FRAME_SIZE             160
#define ENCODED_FRAME_SIZE      20

#define SPEEX_SPI_RCC_APBxPeriphClockCmd		RCC_APB1PeriphClockCmd
#define SPEEX_SPI_RCC_APBxPeriph_SPIx		RCC_APB1Periph_SPI2
#define SPEEX_SPI_SPIx		SPI2
#define SPEEX_SPI_SPI_Direction		SPI_Direction_2Lines_FullDuplex
#define SPEEX_SPI_SPI_Mode		SPI_Mode_Master
#define SPEEX_SPI_SPI_DataSize		SPI_DataSize_8b
#define SPEEX_SPI_SPI_CPOL		SPI_CPOL_Low
#define SPEEX_SPI_SPI_CPHA		SPI_CPHA_1Edge
#define SPEEX_SPI_SPI_NSS		SPI_NSS_Soft
#define SPEEX_SPI_SPI_BaudRatePrescaler		SPI_BaudRatePrescaler_4
#define SPEEX_SPI_SPI_FirstBit		SPI_FirstBit_MSB
#define SPEEX_SPI_SPI_CRCPolynomial		10

//Настройка Mosi
#define SPEEX_MOSI_RCC_APBxPeriphClockCmd		RCC_APB2PeriphClockCmd
#define SPEEX_MOSI_RCC_APBxPeriph_AFIO		RCC_APB2Periph_AFIO
#define SPEEX_MOSI_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOB
#define SPEEX_MOSI_GPIOx		GPIOB
#define SPEEX_MOSI_GPIO_Pin_x		GPIO_Pin_15
#define SPEEX_MOSI_GPIO_Mode		GPIO_Mode_AF_PP
#define SPEEX_MOSI_GPIO_Speed		GPIO_Speed_50MHz

//Настройка Miso
#define SPEEX_MISO_RCC_APBxPeriphClockCmd		RCC_APB2PeriphClockCmd
#define SPEEX_MISO_RCC_APBxPeriph_AFIO		RCC_APB2Periph_AFIO
#define SPEEX_MISO_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOB
#define SPEEX_MISO_GPIOx		GPIOB
#define SPEEX_MISO_GPIO_Pin_x		GPIO_Pin_14
#define SPEEX_MISO_GPIO_Mode		GPIO_Mode_AF_PP
#define SPEEX_MISO_GPIO_Speed		GPIO_Speed_50MHz

//Настройка CLK
#define SPEEX_CLK_RCC_APBxPeriphClockCmd		RCC_APB2PeriphClockCmd
#define SPEEX_CLK_RCC_APBxPeriph_AFIO		RCC_APB2Periph_AFIO
#define SPEEX_CLK_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOB
#define SPEEX_CLK_GPIOx		GPIOB
#define SPEEX_CLK_GPIO_Pin_x		GPIO_Pin_13
#define SPEEX_CLK_GPIO_Mode		GPIO_Mode_AF_PP
#define SPEEX_CLK_GPIO_Speed		GPIO_Speed_50MHz

//Настройка CS
#define SPEEX_CS_RCC_APBxPeriphClockCmd		RCC_APB2PeriphClockCmd
#define SPEEX_CS_RCC_APBxPeriph_AFIO		RCC_APB2Periph_AFIO
#define SPEEX_CS_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOA
#define SPEEX_CS_GPIOx		GPIOA
#define SPEEX_CS_GPIO_Pin_x		GPIO_Pin_9
#define SPEEX_CS_GPIO_Mode		GPIO_Mode_Out_PP
#define SPEEX_CS_GPIO_Speed		GPIO_Speed_2MHz

//Настройка TIM
#define SPEEX_TIM_RCC_APBxPeriphClockCmd		RCC_APB1PeriphClockCmd
#define SPEEX_TIM_RCC_APBxPeriph_TIMx		RCC_APB1Periph_TIM2
#define SPEEX_TIM_TIM_Prescaler		4500
#define SPEEX_TIM_TIM_ClockDivision		TIM_CKD_DIV1
#define SPEEX_TIM_TIM_Period		1
#define SPEEX_TIM_TIM_CounterMode		TIM_CounterMode_Up
#define SPEEX_TIM_TIMx		TIM2
#define SPEEX_TIM_TIMx_IRQn		TIM2_IRQn

//Инициализация библиотеки speex
void Speex_Init(void);

//Настройка переферии
void Speex_SPI_Init(void);

//Воспроизведение звука через массива в памяти микроконтроллера
void play_message(unsigned char const *array, uint16_t frame_number);

//Воспроизведение звука из внешней памяти eeprom
void play_message_from_eeprom(uint16_t address, uint16_t frame_number);

#endif
