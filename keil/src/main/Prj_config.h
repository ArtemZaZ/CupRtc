#ifndef PRJ_CONFIG_H
#define PRJ_CONFIG_H

#include "buttom.h"
#include "freertos.h"
#include "include/cmsis_os.h"

/*общие настройки проекта*/
#define USE_LED_MATRIX      //подключение светодиодов
#define USE_LCD_DISPLAY     //подключение дисплея
#define USE_SPEEX_SPEAKER   //подключение динамика и кодера

#include "uart.h"
#include "cspeexparser.h"

#ifdef USE_LED_MATRIX
	#include "ws2812b.h"
	#include "ws2812b_modes.h"
	
	#define Number_of_Led_modes 2        //Кол-во режимов в файле ws2812b_modes.c
	
	#define WS2812B_NUM_LEDS          9  //Количество светодиодов в ленте
	#define WS2812B_TIMER_AAR    0x0059  //Период таймера (нужно получить 1.25мкс)
	
#endif

#ifdef USE_LCD_DISPLAY
	#include "Lcd.h"
	
#endif

#ifdef USE_SPEEX_SPEAKER
	#include "Spx.h"
#endif

#endif
