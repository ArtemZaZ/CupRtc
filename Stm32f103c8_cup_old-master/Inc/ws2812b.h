/*
 * ws2812b.h
 *
 *  Created on: 9 окт. 2017 г.
 *      Author: user
 */

#ifndef WS2812B_WS2812B_H_
#define WS2812B_WS2812B_H_

#include "stm32f1xx_hal.h"
#include "math.h"
#include "cmsis_os.h"

// Количество светодиодов в первой группе
#define NUM_LED_OF_GROUP_ONE	2
// Количество светодиодов во второй группе
#define NUM_LED_OF_GROUP_TWO	3
// Количество светодиодов в третьей группе
#define NUM_LED_OF_GROUP_THREE	2

#define NUM_LED_OF_GROUP_FOUR	2
// Количество групп светодиодов
#define QUANTITY_OF_GROUPS		4
// Количество светодиодов в ленте
#define QUANTITY_OF_LED  		9

#if((NUM_LED_OF_GROUP_ONE + NUM_LED_OF_GROUP_TWO + NUM_LED_OF_GROUP_THREE + NUM_LED_OF_GROUP_FOUR) != QUANTITY_OF_LED)
{
	#error QUANTITY_OF_LED and sum of NUM_LED_OF_GROUP should be equal!
}
#endif

// Длина низкого уровня для разделения команд
#define TRAILING_BYTES			42 //было 42
// Делитель, настраиваемый при инициализации прерываний для ШИМ
#define COUNTER_PERIOD_PWM		80
// Длина высокого уровня, необходимая для создание логической единицы в WS2812b
#define PWM_FOR_RGB_HIGH		(uint8_t)ceil(0.72*COUNTER_PERIOD_PWM)
// Длина высокого уровня, необходимая для создание логического нуля в WS2812b
#define PWM_FOR_RGB_LOW			(uint8_t)floor(0.28*COUNTER_PERIOD_PWM)


//void WS2812_send(uint8_t redLED,uint8_t greenLED, uint8_t blueLED, uint16_t len);
void WS2812_send_group_short(uint8_t (*groupsColor)[3], uint8_t quantityOfGroups); //не работает
void WS2812_send_noPTR(uint8_t redLED, uint8_t greenLED, uint8_t blueLED, uint16_t len); //цвет по ленте (len - кол-во светодиодов)
void WS2812_send_group(uint8_t redLED1, uint8_t greenLED1, uint8_t blueLED1,
		uint8_t redLED2, uint8_t greenLED2, uint8_t blueLED2,
		uint8_t redLED3, uint8_t greenLED3, uint8_t blueLED3,
		uint8_t redLED4, uint8_t greenLED4, uint8_t blueLED4); //цвет по группам
//какие-то комбинации
void WS2812_combination_1(uint8_t color);
void WS2812_combination_2(uint8_t color);
void WS2812_combination_3(void);

#endif /* WS2812B_WS2812B_H_ */
