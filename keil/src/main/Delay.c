#include "Delay.h"

#define MS_PRSC 7200
#define US_PRSC 72

/*
*Инициализация таймера
*/
void Delay_Init (void)
{
	RCC -> APB1ENR |= RCC_APB1ENR_TIM4EN;  //подать тактирование на TIM4
	TIM4 -> CR1 = TIM_CR1_OPM;  //режим одного импульса
}

/*
*Задержка в миллисекундах
*/
void delay_ms(int delay)
{
	TIM4->PSC = MS_PRSC - 1;
	TIM4->ARR = delay * 10; 
	TIM4->EGR |= TIM_EGR_UG;
	TIM4->CR1 |= TIM_CR1_CEN | TIM_CR1_OPM;
	while ((TIM4->CR1 & TIM_CR1_CEN) != 0);
}

/*
*Задержка в микросекундках
*/
void delay_us(int delay)
{
	TIM4->PSC = US_PRSC;
	TIM4->ARR = delay - 1;
	TIM4->EGR |= TIM_EGR_UG;
	TIM4->CR1 |= TIM_CR1_CEN | TIM_CR1_OPM;
	while ((TIM4->CR1 & TIM_CR1_CEN) != 0);
}
