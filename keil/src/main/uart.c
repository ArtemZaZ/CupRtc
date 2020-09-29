#include "uart.h"

/*
*Структуры для работы с полученными по uart-у данными
*/
BlocksPackage blocksPackage;
TextPackage textPackage;
SpeexPackage speexPackage;
FeedbackPackage fbPackage;

/*
*Инициализация Uart-a
*/
void Serial_Init(void)
{
	SERIAL_GPIO_RCC_APBxPeriphClockCmd(SERIAL_GPIO_RCC_APBxPeriph_GPIOx, ENABLE);
	GPIO_InitTypeDef RxTx_Struct; 
	//TX
	RxTx_Struct.GPIO_Pin = SERIAL_TX_GPIO_Pin;
	RxTx_Struct.GPIO_Speed = SERIAL_TX_GPIO_Speed;
	RxTx_Struct.GPIO_Mode = SERIAL_TX_GPIO_Mode;
	GPIO_Init(SERIAL_GPIOx, &RxTx_Struct);
	//RX
	RxTx_Struct.GPIO_Pin = SERIAL_RX_GPIO_Pin;
	RxTx_Struct.GPIO_Mode = SERIAL_RX_GPIO_Mode;
	GPIO_Init(SERIAL_GPIOx, &RxTx_Struct);
	
	//UART
	SERIAL_RCC_APBxPeriphClockCmd(SERIAL_RCC_APBxPeriph_USARTx,ENABLE);
	USART_InitTypeDef Usart_Struct;
	Usart_Struct.USART_BaudRate = SERIAL_USART_BaudRate;
	Usart_Struct.USART_WordLength = SERIAL_USART_WordLength;
	Usart_Struct.USART_StopBits = SERIAL_USART_StopBits;
	Usart_Struct.USART_Parity = SERIAL_USART_Parity;
	Usart_Struct.USART_Mode = SERIAL_USART_Mode;
	Usart_Struct.USART_HardwareFlowControl = SERIAL_USART_HardwareFlowControl;
	USART_Init(SERIAL_USARTx, &Usart_Struct);
	USART_Cmd(SERIAL_USARTx, ENABLE);
}

/*
*Чтение массива данных по uart
*/
void Uart_Read_Array(uint8_t * arr, uint32_t size)
{
	while(size > 0){
		while( !USART_GetFlagStatus(SERIAL_USARTx, USART_FLAG_RXNE) );
		*arr = (uint8_t)USART_ReceiveData(SERIAL_USARTx);
		size--;
		arr++;
	}
}

/*
*Отправка массива данных по uart
*/
void Uart_Send_Array(uint8_t * arr, uint32_t size)
{
	for(int i = 0; i < size; i++){
		while( !USART_GetFlagStatus(SERIAL_USARTx, USART_FLAG_TXE) );
		USART_SendData(SERIAL_USARTx, arr[i]);
	}
}

/*
*Примем полного пакета данных по uart
*Ожидает приема сообщения с внешнего источника, после по дескриптору
*определяет что конкретно сейчас было получено
*Полный пакет: кол-во фреймов для speex-а, 10 текстов для дисплея,
*массив для speex-а.
*Все полученные данные отправляются во внешний епром.
*/
void Receiving_Data_Usart(void)
{
	uint8_t buf[160] = {0};
	sendFeedback(Uart_Send_Array, 1); //на случай если передача запущена раньше, чем начал принимать кубок (произойдет перессылка первого пакета)
	uint16_t address_spx = 0;
	uint16_t address_buf = 0;
	uint8_t desc = 0;
	uint8_t numb_of_text = 0;
	while(desc != 4){ //пока не придет сообщение с компа об окончании передачи
		desc = recv(Uart_Read_Array, &blocksPackage, &textPackage, &speexPackage, &fbPackage);
		switch(desc){
			// работа с кол-вом фреймов: blocksPackage.data
			case 1:
				EEPROM_Write_Enable();
				EEPROM_Write_Buffer((uint8_t*)&blocksPackage.data, 2, NUM_FRAME_ADDRESS);
				sendFeedback(Uart_Send_Array, 0);
				break;
			// работа с текстами: textPackage.textnum, textPackage.text
			case 2:
				if(numb_of_text < NUMB_OF_TEXT){
					EEPROM_Write_Enable();
					EEPROM_Write_Buffer(textPackage.text, MAX_TEXT_SIZE, TEXT_ADDRESS + (numb_of_text * 0x20));
					numb_of_text ++;
					sendFeedback(Uart_Send_Array, 0);
				}
				else{
					sendFeedback(Uart_Send_Array, 0);
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
					EEPROM_Write_Buffer(buf, 160, SPEEX_ADDRESS + address_spx);
					address_spx += 160;
				}
				sendFeedback(Uart_Send_Array, 0);
				break;
			// работа с обратной связью(сюда придет собщение об окончании связи)
			case 4:
				sendFeedback(Uart_Send_Array, 0);
				break;
			default:        
				sendFeedback(Uart_Send_Array, 1);
				break;
		}
	}
	EEPROM_Write_Enable();
	EEPROM_Write_Buffer((uint8_t*)&numb_of_text, 2, NUM_TEXT_ADDRESS);
}

/*
*Включение режима записи.
*Запустить можно только при включении платы и зажатой кнопки.
*После успешного включения будет ждать приема данных по uart
*/
void Wait_Data_Reception(void)
{
	uint16_t delay = 0;
	//висим пока нажата кнопка и шлем по юарту фидбэк, чтобы, если комп уже отправлял пакеты, сделал это еще раз
	while(!GPIO_ReadInputDataBit(BUTTOM_GPIOx, BUTTOM_GPIO_Pin_x)){
		sendFeedback(Uart_Send_Array, 1);
		if(USART_GetFlagStatus(SERIAL_USARTx, USART_FLAG_RXNE)){ //и если комп все же ответил начинаем прием данных
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
