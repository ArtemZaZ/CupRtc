#include "uart.h"

StatePackage statePackage;
TextPackage textPackage;
SpeexPackage speexPackage;
FeedbackPackage fbPackage;

void Usart_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_InitTypeDef RxTx; 
	//Tx
	RxTx.GPIO_Pin = GPIO_Pin_2;
	RxTx.GPIO_Speed = GPIO_Speed_2MHz;
	RxTx.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &RxTx);
	//Rx
	RxTx.GPIO_Pin = GPIO_Pin_3;
	RxTx.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &RxTx);
	
	USART_InitTypeDef Usart;
	Usart.USART_BaudRate = 19200;
	Usart.USART_WordLength = USART_WordLength_8b;
	Usart.USART_StopBits = USART_StopBits_1;
	Usart.USART_Parity = USART_Parity_No;
	Usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	Usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART2, &Usart);
	USART_Cmd(USART2, ENABLE);
}

void Usart_read_arr(uint8_t * arr, uint32_t size){
	while(size > 0){
		while( !USART_GetFlagStatus(USART2, USART_FLAG_RXNE) );
		*arr = (uint8_t)USART_ReceiveData(USART2);
		size--;
		arr++;
	}
}

void Usart_send_array(uint8_t * arr, uint32_t size){
	for(int i = 0; i < size; i++){
		while( !USART_GetFlagStatus(USART2, USART_FLAG_TXE) );
		USART_SendData(USART2, arr[i]);
	}
}

void Receiving_Data_Usart(void){
	uint8_t buf[160] = {0};
	sendFeedback(Usart_send_array, 1); //на случай если пеедача запущена раньше, чем начал принимать кубок (произойдет перессылка первого пакета)
	uint16_t address_spx = 0;
	uint16_t address_buf = 0;
	uint8_t desc = 0;
	uint8_t numb_of_text = 1;
	while(desc != 4){ //пока не придет сообщение с компа об окончании передачи
		desc = recv(Usart_read_arr, &statePackage, &textPackage, &speexPackage, &fbPackage);
		switch(desc){
			// работа с местом участника: statePackage.state
			case 1:
				eeprom_write_enable();
				eeprom_write_buffer(&statePackage.state, 1, STATE_ADDRESS);
				sendFeedback(Usart_send_array, 0);
				break;
			// работа с текстами: textPackage.textnum, textPackage.text
			case 2:
				if(numb_of_text <= NUMB_OF_TEXT){
					eeprom_write_enable();
					eeprom_write_buffer(textPackage.text, MAX_TEXT_SIZE, TEXT_ADDRESS * numb_of_text);
					numb_of_text ++;
					sendFeedback(Usart_send_array, 0);
				}
				else{
					sendFeedback(Usart_send_array, 0);
				}
				break;
			// работа с блоком speex: speexPackage.data
			case 3:
				//в епром отправляем по 160 байт, чтобы равномерно заполнять страницы памяти,
				//т.к. 160 делиться и на 20(размер фрейма) и на 32 (размер страницы)
				//и получится в 160 байтах 8 фреймов расположенных на 5 страницах
				for(int i = 0; i < 20; i++){
					buf[address_buf + i] = speexPackage.data[i];
				}
				address_buf+=20;
				if(address_buf >= 150){
					address_buf = 0;
					eeprom_write_buffer(buf, 160, SPEEX_ADDRESS + address_spx);
					address_spx += 160;
				}
				sendFeedback(Usart_send_array, 0);
				break;
			// работа с обратной связью(сюда придет собщение об окончании связи)
			case 4:
				sendFeedback(Usart_send_array, 0);
				break;
			default:        
				sendFeedback(Usart_send_array, 1);
				break;
		}
	}
}

void wait_data_reception(void){
	uint16_t delay = 0;
	//висим пока нажата кнопка и шлем по юарту фидбэк, чтобы, если комп уже отправлял пакеты, сделал это еще раз
	while(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)){
		sendFeedback(Usart_send_array, 1);
		if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE)){ //и если комп все же ответил начинаем прием данных
			Receiving_Data_Usart();
			break;
		}
		delay_ms(10);  //или ждем так 3 секунды и переходим в обычный режим работы
		delay+=10;
		if(delay >= 300){
			break;
		}
	}
}
