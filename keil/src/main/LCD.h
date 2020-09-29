#ifndef LCD_H
#define LCD_H

/*
*Библиотека для работы с LCD-дисплеем Winstar WH1602B
*/

#include "stm32f10x_gpio.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "Delay.h"  //для использования задержек

//Общие настройки для всех выводов
#define LCD_RCC_APBxPeriph_AFIO		RCC_APB2Periph_AFIO
#define LCD_RCC_APBxPeriphClockCmd		RCC_APB2PeriphClockCmd
#define LCD_GPIO_Mode		GPIO_Mode_Out_PP
#define LCD_GPIO_Speed		GPIO_Speed_50MHz

//Настрйока вывода DB4
#define LCD_DB4_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOA
#define LCD_DB4_GPIOx		GPIOA
#define LCD_DB4_GPIO_Pin_x		GPIO_Pin_12
//Настрйока вывода DB5
#define LCD_DB5_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOA
#define LCD_DB5_GPIOx		GPIOA
#define LCD_DB5_GPIO_Pin_x		GPIO_Pin_15
//Настрйока вывода DB6
#define LCD_DB6_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOB
#define LCD_DB6_GPIOx		GPIOB
#define LCD_DB6_GPIO_Pin_x		GPIO_Pin_3
//Настрйока вывода DB7
#define LCD_DB7_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOB
#define LCD_DB7_GPIOx		GPIOB
#define LCD_DB7_GPIO_Pin_x		GPIO_Pin_4

//Настрйока вывода RS
#define LCD_RS_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOA
#define LCD_RS_GPIOx		GPIOA
#define LCD_RS_GPIO_Pin_x		GPIO_Pin_10
//Настрйока вывода EN
#define LCD_EN_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOA
#define LCD_EN_GPIOx		GPIOA
#define LCD_EN_GPIO_Pin_x		GPIO_Pin_11

//Инициализация дисплея
void LCD_Init(void);

//Сброс всех выводов
void LCD_GPIO_Reset_All(void);

//Установка полубайта на шине данных
void LCD_GPIO_Set_Half_Byte(uint8_t byte);

//Сигнал чтения/записи
void LCD_Pulse_EN(void);

//Отправка байта корманды/данных на дисплей
void LCD_Send_Byte(char ByteToSend, uint8_t IsData);

//Очистка дисплея
void LCD_Clear_Screen(void);

//Зачистка куска дисплея
void LCD_Cleaning (int row, int col, int number);

//Установка позиции курсора
void LCD_Set_Cursor(char Row, char Col);

//Печать строки
void LСD_Print_Str(char *Text);

//Печать переменной
void LCD_Print_Var(int y);

//Сигнал чтения/записи для работы во 
//void LCD_Pulse_EN_For_FreeRTOS(void);


#endif
