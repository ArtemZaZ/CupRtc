#include "cspeexparser.h"

static uint8_t recvBuffer[50];

uint8_t recv(void (*readArrayFun)(uint8_t*, uint32_t), StatePackage* state, TextPackage* text, SpeexPackage* speex)
{
    readArrayFun(recvBuffer, 20);
    return 0;
}

void send(FeedbackPackage* f)
{

} 
