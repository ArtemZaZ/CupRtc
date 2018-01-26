/*
 * conspeex.h
 *
 *  Created on: 22.05.2017
 *      Author: artem
 */

#ifndef CONSPEEX_H_
#define CONSPEEX_H_
#include <stdint.h>


#define STATE 1
#define TEXT1 2
#define TEXT2 3
#define BLOCK 4
#define SPEEX 5
#define ERROR 6
#define BUFLEN 100

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


#endif /* CONSPEEX_H_ */
