#ifndef SPX_H
#define SPX_H

#include <speex/speex.h>
#include <stdint.h>
#include "spi.h"
#include "EEPROM.h"
#include "Prj_config.h"

#define FRAME_SIZE             160
#define ENCODED_FRAME_SIZE      20

//Инициализация библиотеки speex
void Speex_Init(void);
//Воспроизведение звука через массива в памяти микроконтроллера
void play_message(unsigned char const *array, uint16_t frame_number);
//Воспроизведение звука из внешней памяти eprom
void play_message_from_eeprom(uint16_t address, uint16_t frame_number);

#endif
