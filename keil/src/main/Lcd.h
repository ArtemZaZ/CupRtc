#ifndef LCD_H
#define LCD_H

#include "mcu_support_package/inc/stm32f10x.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "Delay.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "math.h"
#include "Prj_config.h"

//Импульс на ноге Е для записи дисплеем заданных параметров
void PulseLCD(void);
//Отсылка байта в дисплей 
void LcdSendByte(char ByteToSend, int IsData);
//Установка позиции курсора
void LcdCursor(char Row, char Col);
//Очистка дисплея 
void LcdClearScreen(void);
//Инициализация дисплея 
void LCD_Init(void);
//Печать строки 
void LcdPrintStr(char *Text);
//Печать переменной
void LcdPrintVar(int x);
//Очистка куска дисплея
void LcdCleaning (int row, int col, int number);


#endif
