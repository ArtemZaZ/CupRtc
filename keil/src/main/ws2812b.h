#ifndef WS2812B_H
#define WS2812B_H

#include "Prj_config.h"

#include <stdint.h>
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>
#include <stm32f10x_dma.h>
#include <misc.h>

//Инициализация интерфейса ws2812b
void ws2812b_init(void);

//Очистить буфер светодиодной ленты.
void ws2812b_buff_clear(void);

//Установить компоненты RGB светодиода номер pixn
//Возвращаемые значения
// 0 - выполнено успешно
// 1 - неверное значение pixn
int ws2812b_set(
			int pixn, /*номер светодиода*/ 
				uint8_t r,/*красный цвет 0..255*/ 
			uint8_t g,  /*зеленый цвет 0..255*/ 
			uint8_t b); /*синий цвет 0..255*/

//Загрузить подготовленный буфрер в светодиодную ленту.
//Возврашает 1 если предыдущая операция обмена данными еще не завершена.
int ws2812b_send(void);

//Возвращает 1 если предыдущая операция обмена данными с светодиодной лентой 
//завершена успешно
int ws2812b_is_ready(void);

//структура в которую записывается результат выполнения функции 
//void HSV(int H, int S, int V)
struct rgb_struct{
	int r;
	int g;
	int b;
};

//Перевод цвета из модели HSV в модель RGB, которую принимают светодиоды
void HSV(int H, int S, int V);
#endif

