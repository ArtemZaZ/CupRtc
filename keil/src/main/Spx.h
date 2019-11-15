#ifndef SPX_H
#define SPX_H

#include <speex/speex.h>
#include <stdint.h>
#include "spi.h"
#include "EEPROM.h"
#include "Prj_config.h"

void Speex_Init(void);

//void play_message(unsigned char const *array, uint16_t frame_number);

void play_message_from_eeprom(uint16_t address, uint16_t frame_number);
void decoding_speex_buf(void);
#endif
