/*
 * conspeex.h
 *
 *  Created on: 22.05.2017
 *      Author: artem
 */

#ifndef CONSPEEX_H_
#define CONSPEEX_H_
#include <stdint.h>

#define BUFLEN 100

typedef enum enumComand{
	STATE,
	TEXT1,
	TEXT2,
	BLOCK,
	SPEEX,
	ERROr																// чет с ERROR решить, пишет уже определено такое значение уже
}Comand;

typedef enum enumFormat{
	BIGEN,															// порядок хранения данных. Порядок от старшего к младшему (англ. big-endian — большим концом)
	LITTLEN															// Порядок от младшего к старшему (англ. little-endian)
}Format;

typedef struct{
	char command;
	char blockNumber[5];  													// один дополнительный байт взят, для удобства распарсивания
	char data[BUFLEN];
	char hesh[4];
} str_speex_data;


typedef struct{
	uint8_t command;														// комманда
	uint16_t blockNumber;													// *****
	uint8_t data[BUFLEN];													// поле данных
} speex_data;


uint8_t ConvertSymbolToNumber(uint8_t sym);									// переводит символ в цифру
uint8_t ConvertStringToNumber(uint8_t* str, uint8_t* ret, uint8_t n, Format format);    // переводит строку в число, str - указатель на входную строку, ret - указатель на выходную строку, n - число символов, которые нужно перевести, format - формат хранения данных
uint8_t ui8ncmp(uint8_t* str1, uint8_t* str2, uint8_t n);			// сравнивает две строки uint8_t
uint16_t calcrc(uint8_t *ptr, uint8_t count);									// До сих пор не знаю, как работает, взял из какой-то оффиц. либы
Comand ComandDefiner(uint8_t* comand);											// сравнивает первые 5 байт строки comand c возможными командами

speex_data parsing(char*, uint8_t);													// парсинг данных(пользовательская ф-ия)





#endif /* CONSPEEX_H_ */
