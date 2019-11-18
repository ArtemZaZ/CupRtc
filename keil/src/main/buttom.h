#ifndef BUTTOM_H
#define BUTTOM_H

#include "Prj_config.h"
#include <stdint.h>
#include "stm32f10x_gpio.h"

#define NUMBER_OF_POLLS 10

//Инициализация кнопки
void InitButtom(void);
//Устранение дребезга кнопки
uint8_t AntiContactBounce(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif
