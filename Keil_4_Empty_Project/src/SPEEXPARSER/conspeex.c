/*
 * conspeex.c
 *
 *  Created on: 22.05.2017
 *      Author: artem
 */

#include "conspeex.h"
#include <stdint.h>



uint8_t ConvertSymbolToNumber(uint8_t sym)																				// переводит символ в цифру вне зависимости от кодировки
{
	if((sym>=(uint8_t)'0')&&(sym<=(uint8_t)'9')) return sym - (uint8_t)'0';					// для цифр
	if((sym>=(uint8_t)'A')&&(sym<=(uint8_t)'F')) return (sym - (uint8_t)'A') + 10;	// для букв
	else return 0;																																	// потом придумаю
}

uint8_t ConvertStringToNumber(uint8_t* str, uint8_t* ret, uint8_t n, Format format)    // переводит строку в число, str - указатель на входную строку, ret - указатель на выходную строку, 
{                                                                                      // n - число символов, которые нужно перевести, format - формат хранения данных
	switch(format)
	{
		case LITTLEN:
			for(uint8_t i=0; i<(n>>1); i++)
			{
				ret[(n>>1)-i-1] = (ConvertSymbolToNumber(str[i<<1])<<4) + ConvertSymbolToNumber(str[(i<<1)+1]);
			}
			return 1;

		case BIGEN:
			for(uint8_t i=0; i<(n>>1); i++)
			{
				ret[i] = (ConvertSymbolToNumber(str[i<<1])<<4) + ConvertSymbolToNumber(str[(i<<1)+1]);
			}
			return 1;

		default:
			return 0;
	}
}

uint8_t ui8ncmp(uint8_t* str1, uint8_t* str2, uint8_t n)			// сравнивает две строки uint8_t(только равно)
{
	while((*str1++ == *str2++)&&(--n>0));
	if(n==0)	return 1;
	else 			return 0;
}

Comand ComandDefiner(uint8_t* com)    // возвращает команду из перечисления(костыль, мб переделаю потом) 
{     	
	if(ui8ncmp(com, (uint8_t*)"STATE", 5)) 	return STATE; 
	if(ui8ncmp(com, (uint8_t*)"TEXT1", 5))	return TEXT1; 
	if(ui8ncmp(com, (uint8_t*)"TEXT2", 5))	return TEXT2; 
	if(ui8ncmp(com, (uint8_t*)"BLOCK", 5)) 	return BLOCK; 
	if(ui8ncmp(com, (uint8_t*)"SPEEX", 5)) 	return SPEEX; 
	return ERROr;
}




uint16_t calcrc(uint8_t *ptr, uint8_t count)   // вычисление хеш-суммы crc16Xmodem
{
	uint16_t crc;
	uint8_t i;
    crc = 0;
    while (count--!=0)
    {
        crc = crc ^ *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;

        } while(--i);
    }
    return (crc);
}
