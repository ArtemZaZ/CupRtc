#include "buttom.h"

/*
*Функция инициализации кнопки на первом пине порта А
*/
void InitButtom(void)
{
	BUTTOM_RCC_APBxPeriphClockCmd(BUTTOM_RCC_APBxPeriph_GPIOx, ENABLE);
	GPIO_InitTypeDef Reset_Buttom_Struct;
	Reset_Buttom_Struct.GPIO_Mode = BUTTOM_GPIO_Mode;
	Reset_Buttom_Struct.GPIO_Pin = BUTTOM_GPIO_Pin_x;
	Reset_Buttom_Struct.GPIO_Speed = BUTTOM_GPIO_Speed;
	GPIO_Init(BUTTOM_GPIOx, &Reset_Buttom_Struct);
}

/*
*Функция устранения дребезга кнопки
*/
uint8_t AntiContactBounce(void)
{
	uint8_t numOfPolls;
	numOfPolls = 1;
	while(!GPIO_ReadInputDataBit(BUTTOM_GPIOx, BUTTOM_GPIO_Pin_x))
	{
		numOfPolls++;
		osDelay(1);		//<-дилей фриртоса чтобы система не висла
		if(numOfPolls >= NUMBER_OF_POLLS)
			return Bit_SET;
	}
	return Bit_RESET;
}
