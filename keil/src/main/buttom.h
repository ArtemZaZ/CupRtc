#ifndef BUTTOM_H
#define BUTTOM_H

#include "Prj_config.h"
#include <stdint.h>
#include "stm32f10x_gpio.h"

void InitButtom(void);

uint8_t AntiContactBounce(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

#endif
