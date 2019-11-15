#include "EEPROM.h"


void SPIinit(void){
	spi_init();
	//проверка флага на то что инициализация spi прошла успешно
	if(SPI_I2S_GetFlagStatus(SPI2,SPI_FLAG_MODF) == 1){
		while(1);
	}
}

/*
*Функция отправки байта по spi
*/
void eeprom_send_byte(uint16_t inst){
	SPI_I2S_SendData(SPI2, inst);
	//{Хазанский Р.Р.(авторитет) сказал проверять все три флага после отправки
	while( !SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
	while( !SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE));
	while( SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY));
}

/*
*Функиця получения байта по spi
*/
uint16_t eeprom_read_byte(void){
	//при плучении байта необходимо генерировать такт на ноге CLOCK
	//единсвенный способ это делать - отрпавлять по spi 0
	SPI_I2S_SendData(SPI2, 0); 
	while( !SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_RXNE ));
	while( SPI_I2S_GetFlagStatus( SPI2, SPI_I2S_FLAG_BSY ));
	return SPI_I2S_ReceiveData(SPI2);
}

/*
*Отправка команды на разрешение записи
*Важно: согласно даташиту бит разрешения записи в регистре состояния будет
*сброшен в следующих случаях: после подачи питания, после выполнения команды
*WRDI (запрет записи), после выполнения WRSR (после записи регистра состояния),
*после завершения записи в память.
*/
void eeprom_write_enable(void){
	EEPROM_CS_LOW();
	eeprom_send_byte( WREN );
	EEPROM_CS_HIGH();
	delay_ms(5);
}

/*
*Отрпавлка команды на запрет записи
*/
void eeprom_write_disable(void){
	EEPROM_CS_LOW();
	eeprom_send_byte( WRDI );
	EEPROM_CS_HIGH();
	delay_ms(5);
}

/*
*Получение регистра состояния, 8 бит
*b7,b6,b5,b4,b3,b2,b1,b0
*b7 - SWRD - защита от записи в регист состояний, а также вместе с ногой W могут включить
*аппаратную защиту и работать только на чтение данных
*b6,b5,b4 - не используются, всегда равны 0
*b3,b2 - BP1, BP0 - определяют область памяти для аппаратной защиты, в случае ее включения
*b1 - WEL - бит разрешения записи
*b0 - WIP - показывает занята ли память каким-нибудь циклом записи
*/
uint16_t read_status_register(void){
	EEPROM_CS_LOW();
	eeprom_send_byte( RDSR );
	uint16_t res = 0;
	res = eeprom_read_byte();
	EEPROM_CS_HIGH();
	delay_ms(5);
	return res;
}

/*
*Функция записи регистра состояний. Позволяет управлять битами SWRD,BP1,BP0.
*Разрешать запись этой (бит WEL) командой нет смцысла, т.к. после выполнения функции
*этот бит автоматически сбросится.
*/
void write_status_register(uint8_t reg){
	EEPROM_CS_LOW();
	eeprom_send_byte(WRSR);
	eeprom_send_byte(reg);
	EEPROM_CS_HIGH();
	delay_ms(5);
}

/*
*Внутренняя функция отправки 16-ти битного адреса в eeprom. Сначала старший байт, 
*потом младший.
*/
void eeprom_send_address(uint16_t address){
	eeprom_send_byte(address >> 8);
	eeprom_send_byte(address);
}

/*
*Функиця обычного чтения по адресу из eeprom-а
*/
void eeprom_write_page(uint8_t *buf, uint16_t size, uint16_t address){
	eeprom_write_enable();
	EEPROM_CS_LOW();
	eeprom_send_byte(WRITE);
	eeprom_send_address(address);
	delay_ms(5);
	for(int i = 0; i < size; i++){
		eeprom_send_byte(buf[i]);
	}
	EEPROM_CS_HIGH();
	delay_ms(5);
}

/*
*/
void eeprom_read_buffer(uint8_t *buf, uint16_t size, uint16_t address){
	EEPROM_CS_LOW();
	eeprom_send_byte(READ);
	eeprom_send_address(address);
	//перед чтением буфера очистим регистр приема
	SPI_I2S_ReceiveData(SPI2);
	for(int i = 0; i < size; i++){
		buf[i] = eeprom_read_byte();
	}
	EEPROM_CS_HIGH();
}
/*
*Функция чтения массива с учетом страниц
*Данная функция скомунизжена, но работает исправно
*/
void eeprom_write_buffer(uint8_t *buf, uint16_t size, uint16_t address){
	
	uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0/*, count = 0, temp = 0*/;
	uint16_t sEE_DataNum = 0;
	
	Addr = address % EEPROM_PAGESIZE;
	//count = EEPROM_PAGESIZE - Addr;
	NumOfPage =  size / EEPROM_PAGESIZE;
	NumOfSingle = size % EEPROM_PAGESIZE;
	if (Addr == 0) { /* WriteAddr is EEPROM_PAGESIZE aligned  */
		if (NumOfPage == 0) { /* NumByteToWrite < EEPROM_PAGESIZE */
			sEE_DataNum = size;
			eeprom_write_page(buf, sEE_DataNum, address);
		} 
		else { /* NumByteToWrite > EEPROM_PAGESIZE */
			while (NumOfPage--) {
				sEE_DataNum = EEPROM_PAGESIZE;
				eeprom_write_page(buf, sEE_DataNum, address);
				//delay_ms(5);
				address +=  EEPROM_PAGESIZE;
				buf += EEPROM_PAGESIZE;
			}
			sEE_DataNum = NumOfSingle;
			eeprom_write_page(buf, sEE_DataNum, address);
		}
	}
	/*
	else { // WriteAddr is not EEPROM_PAGESIZE aligned  
		if (NumOfPage == 0) { // NumByteToWrite < EEPROM_PAGESIZE 
			if (NumOfSingle > count) { // (NumByteToWrite + WriteAddr) > EEPROM_PAGESIZE 
				temp = NumOfSingle - count;
				sEE_DataNum = count;
				eeprom_write_page(buf, sEE_DataNum, address);
				address +=  count;
				buf += count;
				sEE_DataNum = temp;
				eeprom_write_page(buf, sEE_DataNum, address);
			} 
			else {
				sEE_DataNum = size;
				eeprom_write_page(buf, sEE_DataNum, address);
			}
		} 
		else { // NumByteToWrite > EEPROM_PAGESIZE
			size -= count;
			NumOfPage =  size / EEPROM_PAGESIZE;
			NumOfSingle = size % EEPROM_PAGESIZE;
			sEE_DataNum = count;
			eeprom_write_page(buf, sEE_DataNum, address);
			address +=  count;
			buf += count;
			while (NumOfPage--) {
				sEE_DataNum = EEPROM_PAGESIZE;
				eeprom_write_page(buf, sEE_DataNum, address);
				address +=  EEPROM_PAGESIZE;
				buf += EEPROM_PAGESIZE;
			}
			if (NumOfSingle != 0) {
				sEE_DataNum = NumOfSingle;
				eeprom_write_page(buf, sEE_DataNum, address);
			}
		}
	}
*/
}
