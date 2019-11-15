#ifndef EEPROM_H
#define EEPROM_H

#include "stm32f10x_spi.h"
#include "main/spi.h"
#include "main/Delay.h"
#include "main/Lcd.h"

#define EEPROM_CS_HIGH() GPIO_SetBits(GPIOB, GPIO_Pin_12)
#define EEPROM_CS_LOW() GPIO_ResetBits(GPIOB, GPIO_Pin_12)
#define EEPROM_PAGESIZE 32

//Inctruction from datasheet
#define WREN  0x06   //Write enable
#define WRDI  0x04   //Write disable
#define RDSR  0x05   //Read status register
#define WRSR  0x01   //Write status register
#define READ  0x03   //Read from memory array
#define WRITE 0x02   //Write to memory array

//Инициализация SPI
void SPIinit(void);

//Отправка байта в eeprom
void eeprom_send_byte(uint16_t inst);

//Чтение байта из eeprom
uint16_t eeprom_read_byte(void);

//Инструкция включения записи 
void eeprom_write_enable(void);

//Инструкция отключения записи
void eeprom_write_disable(void);

//Получение регистра состояния eeprom
uint16_t eeprom_read_status_register(void);

//Запись регистра состояний eeprom
void eeprom_write_status_register(uint8_t reg);

//Отправка буфферва в eeprom с контролем страниц
void eeprom_write_buffer(uint8_t *buf, uint16_t size, uint16_t address);

//Чтение буфера даннызх из eeprom
void eeprom_read_buffer(uint8_t *buf, uint16_t size, uint16_t address);

//Отправка массива в eeprom
void eeprom_write_page(uint8_t *buf, uint16_t size, uint16_t address);

#endif
