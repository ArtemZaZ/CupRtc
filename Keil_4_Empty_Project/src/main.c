#include "CMSIS/stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>
#include "SPEEXPARSER/conspeex.h"

uint64_t temp = 0;
uint8_t buffer[100];


int main(void)       		//В функции main поочередно присвойте каждой из целочисленных переменных следующие значения:        
{											
	//ConvertStringToNumber((uint8_t*)"FABC1270", buffer, 8, LITTLEN);
	//temp = *(uint64_t*)buffer;
	//temp = ui8ncmp((uint8_t*)"AVCSa", (uint8_t*)"AVCSb", 5);
	Comand a = ComandDefiner((uint8_t*)"BLOCK");
	temp = (uint8_t)a;
											 
	while(1)
	{
		;		
	}
	
}
