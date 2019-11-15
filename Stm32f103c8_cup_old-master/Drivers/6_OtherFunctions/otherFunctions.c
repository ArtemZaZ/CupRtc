/*
 * otherFunctions.c
 *
 *  Created on: 21 дек. 2017 г.
 *      Author: root
 */
#include "otherFunctions.h"


// Размер(в байтах) всех полученных данных
uint16_t allDataSize;
// Число страниц флеш памяти
uint8_t pageNum = 0;
// Сообщает нам о том, была ли уже очищена первая страница
uint8_t firstErase = 0;

uint8_t textRecCount;

GPIO_PinState AntiContactBounce(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	uint8_t numOfPolls;
	numOfPolls = 1;
	while(!HAL_GPIO_ReadPin(GPIOx, GPIO_Pin))
	{
		numOfPolls++;
		osDelay(1);
		if(numOfPolls >= NUMBER_OF_POLLS)
			return GPIO_PIN_SET;
	}
	return GPIO_PIN_RESET;
}

/*----------------------------------------------------------------------------*/
/*--------------------------------FLASH---------------------------------------*/
/*----------------------------------------------------------------------------*/

