#ifndef DELAY_H
#define DELAY_H

/*
*Библиотека для выполнения команд delay в микросекундах и микросекундах
*/

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"


//Инициализация
void Delay_Init(void);

//Задержка в микросекундах
void delay_us(int delay);

//Задержка в миллисекундках
void delay_ms(int delay);

#endif
