/*
 * conspeex.h
 *
 *  Created on: 22.05.2017
 *      Author: artem
 */

#ifndef CONSPEEX_H_
#define CONSPEEX_H_
#include <stdint.h>
#define BUFLEN 100				// размер вспомогательного буффера

typedef enum enumCommand
{
	STATE,
	TEXT1,
	TEXT2,
	TEXT3,
	TEXT4,
	TEXT5,
	TEXT6,
	TEXT7,
	TEXT8,
	TEXT9,
	TEXT10,
	TEXT11,
	TEXT12,
	TEXT13,
	TEXT14,
	TEXT15,
	TEXT16,
	BLOCK,
	SPEEX,
	ERR
}	Command;	


typedef enum enumFormat{
	BIGEN,															// порядок хранения данных. Порядок от старшего к младшему (англ. big-endian — большим концом)
	LITTLEN															// Порядок от младшего к старшему (англ. little-endian)
} Format;

typedef struct{
	Command command;																// комманда
	uint16_t blockNumber;														// *****
	uint8_t data[BUFLEN];														// поле данных
} RecData;

/*****
 * Поле данных blockNumber для комманды SPEEX хранит номер блока
 * Для комманд TEXT1 и TEXT2 хранит кол-во символов, находящихся в поле data
 * Для комманды BLOCK хранит общее количество блоков
*/

static uint8_t convertSymbolToNumber(char sym);				// Переводит символ в число
static uint8_t convertStringToNumber(char* str, uint8_t* ret, uint8_t size, Format format); // переводит строку в число, ret - указатель на область памяти, где хранится число
static Command commandDefiner(char *str);							// Возвращает команду, которая записана в строке. str - указатель на начало комманды 
static uint16_t calcrc(char *ptr, uint8_t count);		  // вычисление хеш-суммы crc16Xmodem
RecData parsing(char* str, uint8_t size);			  // парсер строки данных



#endif /* CONSPEEX_H_ */
