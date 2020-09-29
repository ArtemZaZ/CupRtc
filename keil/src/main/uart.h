#ifndef UART_H
#define UART_H

/*
*Библиотека для обмена данными по последовательному порту с ПК
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x_usart.h"
#include "cspeexparser.h"
#include "Delay.h"
#include "buttom.h"
#include "EEPROM.h"

//Настрйока UART
#define SERIAL_RCC_APBxPeriphClockCmd		RCC_APB1PeriphClockCmd
#define SERIAL_RCC_APBxPeriph_USARTx		RCC_APB1Periph_USART2
#define SERIAL_USARTx		USART2
#define SERIAL_USART_BaudRate		19200
#define SERIAL_USART_WordLength		USART_WordLength_8b
#define SERIAL_USART_StopBits		USART_StopBits_1
#define SERIAL_USART_Parity		USART_Parity_No
#define SERIAL_USART_Mode		USART_Mode_Rx | USART_Mode_Tx
#define SERIAL_USART_HardwareFlowControl		USART_HardwareFlowControl_None

//Общие настройки для обоих выводов
#define SERIAL_GPIO_RCC_APBxPeriphClockCmd		RCC_APB2PeriphClockCmd
#define SERIAL_GPIO_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOA
#define SERIAL_GPIOx		GPIOA
#define SERIAL_TX_GPIO_Speed		GPIO_Speed_2MHz
//Настрйока вывода Tx
#define SERIAL_TX_GPIO_Pin		GPIO_Pin_2 
#define SERIAL_TX_GPIO_Mode		GPIO_Mode_AF_PP
//Настрйока вывода Rx
#define SERIAL_RX_GPIO_Pin		GPIO_Pin_3 
#define SERIAL_RX_GPIO_Mode		GPIO_Mode_IN_FLOATING

//Инициализация Usart2 
void Serial_Init(void);
//Чтение массива по usart
void Uart_Read_Array(uint8_t * arr, uint32_t size);
//Отправлка массива по usart
void Uart_Send_Array(uint8_t * arr, uint32_t size);
//Функиця приема пакета данных для кубка
void Receiving_Data_Usart(void);
//Проверка на перезапись данных для кубка при его включении
void Wait_Data_Reception(void);

#endif
