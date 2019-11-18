#include "buttom.h"


/*
*Функция инициализации кнопки на первом пине порта А
*/
void InitButtom(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIO_InitTypeDef ResetButtom;
	ResetButtom.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	ResetButtom.GPIO_Pin = GPIO_Pin_1;
	ResetButtom.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init( GPIOA, &ResetButtom);
}


/*
*Функция устранения дребезга кнопки
*/
uint8_t AntiContactBounce(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	uint8_t numOfPolls;
	numOfPolls = 1;
	while(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1))
	{
		numOfPolls++;
		osDelay(1);
		if(numOfPolls >= NUMBER_OF_POLLS)
			return Bit_SET;
	}
	return Bit_RESET;
}
