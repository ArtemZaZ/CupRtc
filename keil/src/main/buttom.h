#ifndef BUTTOM_H
#define BUTTOM_H

/*
*Библиотека для работы с кнопкой
*ВНИМАНИЕ
*вся библиотека подстроена под работу с системой freertos
*/

#include "Prj_config.h"
#include <stdint.h>
#include "stm32f10x_gpio.h"

//Настройка пина кнопки
#define BUTTOM_RCC_APBxPeriphClockCmd		RCC_APB2PeriphClockCmd
#define BUTTOM_RCC_APBxPeriph_GPIOx		RCC_APB2Periph_GPIOA
#define BUTTOM_GPIOx		GPIOA
#define BUTTOM_GPIO_Pin_x		GPIO_Pin_1
#define BUTTOM_GPIO_Mode		GPIO_Mode_IN_FLOATING
#define BUTTOM_GPIO_Speed		GPIO_Speed_10MHz

//Кол-во тиков для определения состояния кнопки
#define NUMBER_OF_POLLS 10

//Инициализация кнопки
void InitButtom(void);

//Устранение дребезга кнопки
uint8_t AntiContactBounce(void);

#endif
