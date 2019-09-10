#include <stdio.h>
#include "cspeexparser.h"

StatePackage state;
TextPackage text;
SpeexPackage speex;

void readArr(uint8_t* out, uint32_t size)
{
    out[0] = 0;
    out[1] = 1;
    printf("recv\n");
}

void sendArr(uint8_t* in, uint32_t size)
{
    printf("send\n");
}

int main (void)
{
    //uint8_t desc = recv(((void*)readArr), &state, &text, &speex);
    sendFeedback((void*)sendArr, 1);
    printf("OK!");
}


