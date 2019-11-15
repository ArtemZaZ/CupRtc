#ifndef DELAY_H
#define DELAY_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

#define MS_PRSC 7200
#define US_PRSC 72

void Delay_Init(void);      //Инициализация

void delay_us(int delay);   //Задержка в микросекундах

void delay_ms(int delay);   //Задержка в миллисекундках

#endif
