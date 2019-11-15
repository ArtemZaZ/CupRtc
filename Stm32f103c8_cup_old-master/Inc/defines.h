/*
 * defines.h
 *
 *  Created on: 22 дек. 2017 г.
 *      Author: root
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEFINES_H
#define __DEFINES_H

// Если мы определяем макрос LED_MATRIX_ENABLE, то тем самым подрубаем в проекте работу
// с LED-матрицу 8x8
//#define LED_MATRIX_ENABLE

// Если мы определяем макрос AUDIO_ENABLE, то тем самым подрубаем в проект возможность
// воспроизводить голосовое сообщение
#define AUDIO_ENABLE

// Если мы определяем макрос LCD_ENABLE, то тем самым подрубаем в проект возможность
// выводить сообщения на LCD
#define LCD_ENABLE

// Если мы определяем макрос WS2812B_ENABLE, то тем самым подрубаем в проект возможность
// работы с управляемыми светодиодами WS2812B
#define WS2812B_ENABLE

/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
/*---------------------------------MODE---------------------------------------*/
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
#define SYSTEM_MODE_NORMAL		0
#define SYSTEM_MODE_DEMO		1
#define SYSTEM_MODE_UPDATE		2
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/


/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
/*--------------------------------FLASH---------------------------------------*/
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
// Значение, которое должно быть в месте записи занятого места, чтобы включился
// демо-режим
#define MEM_STATUS 				(state == 0xFFFFFFFF)
#define MAX_TEXT_SIZE			40
#define LENGTH_OF_LAST_RECORD	4
// Адрес начальной страницы в памяти, а также
// адрес места в памяти для хранения занятого на соревнованиях места
#define START_FLASH_PAGE       	0x801A000
// Адрес места в памяти для хранения размера Text1
#define SIZE_TEXT1_address     	(START_FLASH_PAGE + LENGTH_OF_LAST_RECORD)
// Адрес места в памяти для хранения Text1
#define TEXT1_address          	(SIZE_TEXT1_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text2
#define SIZE_TEXT2_address     	(TEXT1_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text2
#define TEXT2_address          	(SIZE_TEXT2_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text3
#define SIZE_TEXT3_address     	(TEXT2_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text3
#define TEXT3_address          	(SIZE_TEXT3_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text4
#define SIZE_TEXT4_address     	(TEXT3_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text4
#define TEXT4_address          	(SIZE_TEXT4_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text5
#define SIZE_TEXT5_address     	(TEXT4_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text5
#define TEXT5_address          	(SIZE_TEXT5_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text6
#define SIZE_TEXT6_address     	(TEXT5_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text6
#define TEXT6_address          	(SIZE_TEXT6_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text7
#define SIZE_TEXT7_address     	(TEXT6_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text7
#define TEXT7_address          	(SIZE_TEXT7_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text8
#define SIZE_TEXT8_address     	(TEXT7_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text8
#define TEXT8_address          	(SIZE_TEXT8_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text9
#define SIZE_TEXT9_address     	(TEXT8_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text9
#define TEXT9_address          	(SIZE_TEXT9_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text10
#define SIZE_TEXT10_address     (TEXT9_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text10
#define TEXT10_address          (SIZE_TEXT10_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text11
#define SIZE_TEXT11_address     (TEXT10_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text11
#define TEXT11_address          (SIZE_TEXT11_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text12
#define SIZE_TEXT12_address     (TEXT11_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text12
#define TEXT12_address          (SIZE_TEXT12_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text13
#define SIZE_TEXT13_address     (TEXT12_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text13
#define TEXT13_address          (SIZE_TEXT13_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text14
#define SIZE_TEXT14_address     (TEXT13_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text14
#define TEXT14_address          (SIZE_TEXT14_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text15
#define SIZE_TEXT15_address     (TEXT14_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text15
#define TEXT15_address          (SIZE_TEXT15_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения размера Text16
#define SIZE_TEXT16_address     (TEXT15_address + MAX_TEXT_SIZE)
// Адрес места в памяти для хранения Text16
#define TEXT16_address          (SIZE_TEXT16_address + LENGTH_OF_LAST_RECORD)

// Адрес места в памяти для хранения количесва принятых блоков текста
#define TEXT_REC_COUNT			(TEXT16_address + MAX_TEXT_SIZE)

// Адрес места в памяти для хранения количества фреймов(для аудиофайла)
#define BLOCK_address          	(TEXT_REC_COUNT + LENGTH_OF_LAST_RECORD)
// Адрес места в памяти для хранения аудиофайла
#define SPEEX_address          	(BLOCK_address + LENGTH_OF_LAST_RECORD)

#define STATE_MEMORY_SIZE		 4
#define SIZE_RECORD_OF_TEXT_SIZE 4
#define BLOCK_MEMORY_SIZE		 4

// Размер пакета звуковых данных (в байтах)
#define BLOCK_SIZE              20
// Максимальное число байт, которое может уместиться на странице памяти МК
#define MEMORY_PAGE_SIZE  	   	2048
// Размер буфера, в который записываются принимаемые по USART данные
#define SIZE_BUFF               60
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/


/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
/*--------------------------------BUTTON--------------------------------------*/
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
// Количество опросов состояния кнопки для программного антидребезга
#define NUMBER_OF_POLLS 		10
// Вывод МК, на который будет цепляться кнопка
#define BUTTON_GPIO_PORT        GPIOA
#define BUTTON_GPIO_PIN         GPIO_PIN_1
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/


/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
/*--------------------------------EEPROM--------------------------------------*/
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
#define EEPROM_CS_Pin 			GPIO_PIN_12
#define EEPROM_CS_GPIO_Port 	GPIOB
//#define EEPROM_WP_Pin 			GPIO_PIN_1
//#define EEPROM_WP_GPIO_Port 	GPIOB
//#define EEPROM_HOLD_Pin 		GPIO_PIN_10
//#define EEPROM_HOLD_GPIO_Port 	GPIOB

#define EEPROM_READ_ADDR		0x01
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/


/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
/*--------------------------------GPIO--------------------------------------*/
/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
#define CS1_PORT	GPIOA
#define CS1_PIN		GPIO_PIN_8
#define DAC_CS_Pin				GPIO_PIN_9
#define DAC_CS_GPIO_Port 		GPIOA

/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/



#endif
