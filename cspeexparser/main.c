#include <stdio.h>
#include "cspeexparser.h"
#include "uart_handle.h" 

// statePackage: 0xAAAA10210101

StatePackage statePackage;
TextPackage textPackage;
SpeexPackage speexPackage;

void readArr(uint8_t* out, uint32_t size)
{
  while(size > 0)
  {
    if (uart::uart1.isNewByte())
    {
      *out = uart::uart1.getByte();
      out++;
      size--;
    }
  }
}

void sendArr(uint8_t* in, uint32_t size)
{
  uart::uart1.sendStaticArray(in, size);
}

int main (void)
{
  uart::uart1.init(uart::Pins::UART1_PA9_PA10, 9600);   // инициализация физ интерфейса  
  while(1)
  {
    uint8_t desc = recv(readArr, &statePackage, &textPackage, &speexPackage);  
    switch(desc)
    {
      case 1:
        // работа с местом участника: statePackage.state
        break;
      
      case 2:
        // работа с текстами: textPackage.textnum, textPackage.text
        break;
      
      case 3:
        // работа с блоком speex: speexPackage.data
        break;
        
      default:        
        sendFeedback(sendArr, 0);
        break;
    }
  }
}


