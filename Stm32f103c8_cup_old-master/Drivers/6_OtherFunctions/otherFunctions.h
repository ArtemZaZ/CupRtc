/*
 * otherFunctions.h
 *
 *  Created on: 21 дек. 2017 г.
 *      Author: root
 */

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "cmsis_os.h"
#include "string.h"
#include "defines.h"


/* @brief Brief of AntiContactBounce
 *
 * Функция для проверки контактов на дребезг
 * На вход подаётся имя порта и номер пина
 * На выходе получаем 1 или 0
 */
GPIO_PinState AntiContactBounce(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);



/* OTHERFUNCTIONS_H_ */
