#ifndef WS2812b_MODES_H
#define WS2812b_MODES_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "main/ws2812b.h"
#include "main/Delay.h"
#include "freertos.h"

//Включение какого-нибудь режима на светодиодах
//P.s всябиблиотека подстроена под работу с системой freertos
void Turn_on_Led_mode(uint8_t mode);

#endif
