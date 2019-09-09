#pragma once
#include "stdint.h"

typedef struct
{
    uint16_t checksum;
    uint8_t desc;
    uint8_t state;
} StatePackage;

typedef struct
{
    uint16_t checksum;
    uint8_t desc;
    uint8_t textnum;
    uint8_t text[21];
} TextPackage;

typedef struct
{
    uint16_t checksum;
    uint8_t desc;
    uint8_t data[20];
} SpeexPackage;

typedef struct
{
    uint16_t checksum;
    uint8_t desc;
    uint8_t code;
} FeedbackPackage;

uint8_t recv(void (*readArrayFun)(uint8_t*, uint32_t), StatePackage* state, TextPackage* text, SpeexPackage* speex);
void send(FeedbackPackage* f);

