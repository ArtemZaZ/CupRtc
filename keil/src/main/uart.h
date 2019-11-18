#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x_usart.h"
#include "cspeexparser.h"
#include "Delay.h"
#include "EEPROM.h"

//Инициализация Usart2 
void Usart_Init(void);
//Чтение массива по usart
void Usart_read_arr(uint8_t * arr, uint32_t size);
//Отправлка массива по usart
void Usart_send_array(uint8_t * arr, uint32_t size);
//Функиця приема пакета данных для кубка
void Receiving_Data_Usart(void);
//Проверка на перезапись данных для кубка при его включении
void wait_data_reception(void);

#endif
