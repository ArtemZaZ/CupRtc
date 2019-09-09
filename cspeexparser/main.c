#include <stdio.h>
#include "cspeexparser.h"

StatePackage state;
TextPackage text;
SpeexPackage speex;
FeedbackPackage f;

void readArr(uint8_t* out, uint32_t size)
{
    out[0] = 0;
    out[1] = 1;
    printf("recv");
}

int main (void)
{
    uint8_t desc = recv(((void*)readArr), &state, &text, &speex);
    send(&f);
    printf("OK!");
}
