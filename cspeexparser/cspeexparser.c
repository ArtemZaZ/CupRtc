#include "cspeexparser.h"
#include "stdbool.h"
#include "string.h"

#define RECV_BUFFER_LEN 100
#define SEND_BUFFER_LEN 100
#define START_BYTES     0xAAAA
#define HEAD_SIZE       3   // 3 байта

static uint32_t getSizeByDesc(uint8_t desc)  // получение размера данных по дескриптору
{
    switch(desc)
    {
        case 1:
            return 1;

        case 2:
            return 1 + MAX_TEXT_SIZE;

        case 3:
            return SPEEX_BLOCK_SIZE;

        case 4:
            return 1;

        default:    // неизвестный дескриптор
            return 0;
    }
}

static uint16_t crc16Xmodem(uint8_t* ptr, uint8_t count)    // получение crc16
{
	uint16_t crc;
	uint8_t i;
    crc = 0;
    while (count--!=0)
    {
        crc = crc ^ (uint8_t) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;

        } while(--i);
    }
    return (crc);
}

uint8_t recv(void (*readArrayFun)(uint8_t*, uint32_t), StatePackage* state, TextPackage* text, SpeexPackage* speex)
{
    static uint8_t recvBuffer[RECV_BUFFER_LEN];     // буффер в который читается пакет
    uint8_t* recvBufferCounter = recvBuffer;    // каретка
    uint16_t checksum = 0;  // избыточный код пакета
    uint8_t descriptor = 0;

    while(true)      // ищем вхождение в сообщение
    {
        readArrayFun(recvBuffer, 1);
        if(*recvBufferCounter == ((START_BYTES & 0xFF00) >> 8))
        {
            readArrayFun(recvBufferCounter + 1, 1);
            if(*(recvBufferCounter + 1) == (START_BYTES & 0x00FF))
            {
                recvBufferCounter = recvBufferCounter + 2;
                break;
            }
            else
            {
                return 0;
            }
        }
    }

    readArrayFun(recvBufferCounter, HEAD_SIZE); // читаем заголовок
    checksum = *((uint16_t*)recvBufferCounter); // получаем избыточный код
    recvBufferCounter = recvBufferCounter + 2;  // перескакиваем на след. элемент
    descriptor = *recvBufferCounter;    // получаем дескриптор
    recvBufferCounter++;    // перескакиваем на след. элемент
    if(getSizeByDesc(descriptor) == 0) return 0; // ошибка - неизвестный дескриптор

    readArrayFun(recvBufferCounter, getSizeByDesc(descriptor));
    if(crc16Xmodem(recvBufferCounter, getSizeByDesc(descriptor)) != checksum) return 0; // ошибка избыточного кода

    switch(descriptor)
    {
        case 1:
            state->descriptor = descriptor;
            state->checksum = checksum;
            state->state = *recvBufferCounter;
            break;

        case 2:
            text->descriptor = descriptor;
            text->checksum = checksum;
            text->textnum = *recvBufferCounter;
            recvBufferCounter++;
            strncpy(text->text, recvBufferCounter, MAX_TEXT_SIZE);
            break;

        case 3:
            speex->descriptor = descriptor;
            strncpy(speex->data, recvBufferCounter, SPEEX_BLOCK_SIZE);
            break;

        default:    // неизвестный дескриптор
            return 0;
    }
    return descriptor;
}

void sendFeedback(void (*sendArrayFunc)(uint8_t*, uint32_t), uint8_t code)
{
    static uint8_t sendBuffer[SEND_BUFFER_LEN];
    uint8_t* sendBufferCounter = sendBuffer;
    uint16_t checksum = 0;
    uint8_t descriptor = 4;

    checksum = crc16Xmodem((uint8_t*)&code, 1);

    *((uint16_t*)sendBufferCounter) = (uint16_t)START_BYTES;
    sendBufferCounter = sendBufferCounter + 2;

    *((uint16_t*)sendBufferCounter) = checksum;
    sendBufferCounter = sendBufferCounter + 2;

    *sendBufferCounter = descriptor;
    sendBufferCounter++;

    *sendBufferCounter = code;

    sendArrayFunc(sendBuffer, sizeof(START_BYTES) + HEAD_SIZE + getSizeByDesc(descriptor));
} 

