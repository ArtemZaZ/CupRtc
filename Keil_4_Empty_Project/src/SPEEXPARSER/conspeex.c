/*
 * conspeex.c
 *
 *  Created on: 22.05.2017
 *      Author: artem
 */

#include "conspeex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

uint8_t buffer[BUFLEN];

static uint8_t convertSymbolToNumber(char sym)			// перевод символа(шестнадцатиричного(Только верхний регистр)), в число
{
	if((sym>=(uint8_t)'0')&&(sym<=(uint8_t)'9')) return (uint8_t)sym - (uint8_t)'0';					// Для цифр
	if((sym>=(uint8_t)'A')&&(sym<=(uint8_t)'F')) return ((uint8_t)sym - (uint8_t)'A') + 10;		// для букв
	else return 0;																																						// 
}

static uint8_t convertStringToNumber(char* str, uint8_t* ret, uint8_t size, Format format)					// Переводит шестнадцатиричную символьную строку в числовую строку и записывает ее в область памяти, на которую указывает ret 
{
	switch(format)
	{
		case LITTLEN:
			for(uint8_t i = 0; i < size/2; i++)
			{
				ret[size/2-i-1] = convertSymbolToNumber(str[2*i])*16 + convertSymbolToNumber(str[2*i+1]);
			}
			return 1;
		
		case BIGEN:
			for(uint8_t i = 0; i < size/2; i++)
			{
				ret[i] = convertSymbolToNumber(str[2*i])*16 + convertSymbolToNumber(str[2*i+1]);
			}
			return 1;
		
		default:
			return 0;
	}
}

static Command commandDefiner(char *str)						// str - указатель на начало комманды 
{
	if(strncmp(str, "TEXT10", 6) == 0) return TEXT10;
	if(strncmp(str, "TEXT11", 6) == 0) return TEXT11;
	if(strncmp(str, "TEXT12", 6) == 0) return TEXT12;
	if(strncmp(str, "TEXT13", 6) == 0) return TEXT13;
	if(strncmp(str, "TEXT14", 6) == 0) return TEXT14;
	if(strncmp(str, "TEXT15", 6) == 0) return TEXT15;
	if(strncmp(str, "TEXT16", 6) == 0) return TEXT16;
	if(strncmp(str, "STATE", 5) == 0) return STATE;	
	if(strncmp(str, "TEXT1", 5) == 0) return TEXT1;
	if(strncmp(str, "TEXT2", 5) == 0) return TEXT2;
	if(strncmp(str, "TEXT3", 5) == 0) return TEXT3;
	if(strncmp(str, "TEXT4", 5) == 0) return TEXT4;
	if(strncmp(str, "TEXT5", 5) == 0) return TEXT5;
	if(strncmp(str, "TEXT6", 5) == 0) return TEXT6;
	if(strncmp(str, "TEXT7", 5) == 0) return TEXT7;
	if(strncmp(str, "TEXT8", 5) == 0) return TEXT8;
	if(strncmp(str, "TEXT9", 5) == 0) return TEXT9;
	if(strncmp(str, "BLOCK", 5) == 0) return BLOCK;
	if(strncmp(str, "SPEEX", 5) == 0)	return SPEEX;	
	return ERR;
}

static uint16_t calcrc(char *ptr, uint8_t count)   // вычисление хеш-суммы crc16Xmodem(выцеплена из какой-то либы)
{
	uint16_t crc;
	uint8_t i;
    crc = 0;
    while (count--!=0)
    {
        crc = crc ^ (uint8_t) *ptr++ << 8;
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


RecData parsing(char* str, uint8_t size)
{
	RecData ret;
	while((*str != '<')&&(size > 0)) 					// поиск вхождения(проверка скобок)
	{																					// от начала к концу
		str++;
		size--;
	}
	uint8_t tempSize = size;
	while(tempSize > 0)												// от конца к началу
	{	
		if(str[tempSize] == '>')	break;
		tempSize--;
	}
	if((size == 0)||(tempSize == 0))					// если нет хоть какой-нибудь скобки - вернуть ошибку
	{
		ret.command = ERR;
		return ret;
	}	
	str++; 																		// избавляемся от скобочки(первой)
	
	char* token = strtok(str, " ");						// разбиваем строку на лексемы
	char* tempToken;													// вспомогательная переменная(в теле switch-case нельзя создавать локальные объекты)
	uint8_t textSize = 0;											// вспомогательная переменная
	ret.command = commandDefiner(token);	
	uint8_t tempBuffer[BUFLEN];								// Временный буффер uint8_t
	uint16_t hesh = 0;												// Переменная для хеша
	switch(ret.command)
	{
		case STATE:
			token = strtok(NULL, " ");						// первый токен - место
			hesh = calcrc(token, 2);							// по нему считаем хеш 
			convertStringToNumber(token, ret.data, 2, LITTLEN);	//!	переводим в число и записываем в 1й элемент массива
			token = strtok(NULL, " >");						// второй токен - хеш
			convertStringToNumber(token, tempBuffer, 4, LITTLEN);				// переводим его в число
			if(hesh != *(uint16_t*)tempBuffer)		// если хеш не совпал - возвращаем ошибку
			{
				ret.command = ERR;
				return ret;
			}
			break;
		
		case BLOCK:
			token = strtok(NULL, " ");						// первый токен - количество блоков
			hesh = calcrc(token, 4);							// по нему считаем хеш 
			convertStringToNumber(token, tempBuffer, 4, LITTLEN);		// переводим в область памяти, запихиваем во временный буффер, из временного буффера записываем в blockNumber
			ret.blockNumber	= *(uint16_t*)tempBuffer;				  // количества блоков в BLOCK хранится в поле blockNumber, а не data
			token = strtok(NULL, " >");						// второй токен - хеш
			convertStringToNumber(token, tempBuffer, 4, LITTLEN);		// переводим его в число
			if(hesh != *(uint16_t*)tempBuffer)		// если хеш не совпал - возвращаем ошибку
			{
				ret.command = ERR;
				return ret;
			}
			break;
		
		case SPEEX:
			tempToken = strtok(NULL, " ");									// первый токен - номер блока
			convertStringToNumber(tempToken, tempBuffer, 4, LITTLEN);		// переводим в число
			ret.blockNumber = *(uint16_t*)tempBuffer;				// записываем количество блоков
			token = strtok(NULL, " ");											// второй токен - данные
			convertStringToNumber(token, ret.data, 40, BIGEN);					// переводим спиксовые данные и записываем их сразу в память
			strncpy((char*)tempBuffer, tempToken, 4);				// хеш считается по номеру блока + данные
			tempBuffer[4] = '\0';														// strncat дописывает строку вместо символа конца строки
			strncat((char*)tempBuffer, token, 40);					// 
			hesh = calcrc((char*)tempBuffer, 44);
			token = strtok(NULL, " >");			
			convertStringToNumber(token, tempBuffer, 4, LITTLEN);		// переводим его в число
			if(hesh != *(uint16_t*)tempBuffer)		// если хеш не совпал - возвращаем ошибку
			{
				ret.command = ERR;
				return ret;
			}
			break;
		
		case TEXT1:		//группировка case
		case TEXT2:
		case TEXT3:
		case TEXT4:
		case TEXT5:
		case TEXT6:
		case TEXT7:
		case TEXT8:
		case TEXT9:
		case TEXT10:
		case TEXT11:
		case TEXT12:
		case TEXT13:
		case TEXT14:
		case TEXT15:
		case TEXT16:
			token = strtok(NULL, "&");								// первый токен - текст
			tempToken = strtok(NULL, "&>");						// второй токен - конец текста, начало хеша
			textSize = (uint8_t)(tempToken - token) - 1;	// адрес начала следующего токена минус адрес предыдущего = размер текста
			ret.blockNumber = textSize;								// у текста количество символов хранится в поле blockNumber
			strncpy((char*)(ret.data), token, textSize); 			
			hesh = calcrc(token, textSize);						// считаем хеш 
			convertStringToNumber(tempToken+1, tempBuffer, 4, LITTLEN);		// ++ - убираем лишний пробел
			if(hesh != *(uint16_t*)tempBuffer)				// если хеш не совпал - возвращаем ошибку
			{
				ret.command = ERR;
				return ret;
			}
			break;
		
		default:
			ret.command = ERR;
			break;
	}
	
	return ret;
}
