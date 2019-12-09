#include "freertos.h"

void StartRGBws2812bTask(void const * argument);
void StartButtonTask(void const * argument);
void StartLCDTask(void const * argument);
void ChangeLedMode(void const * argument);

extern uint8_t Led_mode;
extern uint8_t bip[]; 
extern uint16_t bip_size;
extern uint8_t r2d2[]; 
extern uint16_t r2d2_size;

uint8_t Lcd_flag = 1;
int page = 0;
void lcd_buffer_filling(void);
void lcd_page_send(void);

xSemaphoreHandle xSemaphore_button;
xSemaphoreHandle xSemaphore_lcd;

uint8_t page1_text1[16] = {0};
uint8_t page1_text2[16] = {0};
uint8_t page2_text1[16] = {0};
uint8_t page2_text2[16] = {0};
uint8_t page3_text1[16] = {0};
uint8_t page3_text2[16] = {0};
uint8_t page4_text1[16] = {0};
uint8_t page4_text2[16] = {0};
uint8_t page5_text1[16] = {0};
uint8_t page5_text2[16] = {0};

void freertos_init(void){
	lcd_buffer_filling();
	
	vSemaphoreCreateBinary(xSemaphore_button);
	xSemaphoreGive(xSemaphore_button);
	
	vSemaphoreCreateBinary(xSemaphore_lcd);
	xSemaphoreGive(xSemaphore_lcd);
	
	xTaskCreate( (TaskFunction_t) StartButtonTask, "StartButtonTask", 128, NULL, 85, NULL);
	xTaskCreate( (TaskFunction_t) StartRGBws2812bTask, "StartRGBws2812bTask", 128, NULL, 85, NULL);
	xTaskCreate( (TaskFunction_t) StartLCDTask, "StartLCDTask", 128, NULL, 85, NULL);
	xTaskCreate( (TaskFunction_t) ChangeLedMode, "ChangeLedMode", 128, NULL, 85, NULL);
}

void ChangeLedMode(void const * argument){
	while(1){
		Led_mode++;
		if(Led_mode > 2) Led_mode = 0;
		osDelay(60000);
	}
}

void StartLCDTask(void const * argument)
{
	uint8_t num_page = 0;
	eeprom_read_buffer( (uint8_t *)&num_page, 32, 0x20);
	num_page = num_page / 2;
	while(1) {
		if(xSemaphoreTake(xSemaphore_lcd, portMAX_DELAY)){
			page++;
			if(page > num_page) page = 1;
			lcd_page_send();
			xSemaphoreGive(xSemaphore_lcd);
			osDelay(10000);
		}
	}
}

void StartButtonTask(void const * argument)
{
	while(1){
		if(xSemaphoreTake(xSemaphore_button, portMAX_DELAY)){
			if(AntiContactBounce(GPIOA, GPIO_Pin_1)){
				int delay = 0;
				while(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) && delay < 1024){
					if(delay >= 256){
						if(xSemaphoreTake(xSemaphore_lcd, 0) == pdPASS){
							LcdClearScreen();
							LcdCursor(1, 5);
							LcdPrintStr("Audio");
							LcdCursor(0, 0);
							for(int i = 0; i < 4; i++){
								LcdSendByte(0xFF, 1);
							}
						}
						else LcdSendByte(0xFF, 1);
					}
					delay+=64;
					osDelay(64);
				}
				if(delay >= 1024){
					uint16_t num_of_frame = 0;
					eeprom_read_buffer( (uint8_t *)&num_of_frame, 32, 0x00);
					lcd_page_send();
					xSemaphoreGive(xSemaphore_lcd);
					play_message_from_eeprom(SPEEX_ADDRESS, num_of_frame);
				}
				else{
					Led_mode++;
					if(Led_mode > 2) Led_mode = 0;
					lcd_page_send();
					xSemaphoreGive(xSemaphore_lcd);
					play_message(bip, bip_size / 20);
				}
			}
			xSemaphoreGive(xSemaphore_button);
			osDelay(10);
		}
	}
}

void StartRGBws2812bTask(void const * argument) 
{
	while(1){
		Turn_on_Led_mode(Led_mode);
	}
}

void lcd_buffer_filling(void){
	eeprom_read_buffer( (uint8_t *)page1_text1, 32, 0x40);
	eeprom_read_buffer( (uint8_t *)page1_text2, 32, 0x60);
	eeprom_read_buffer( (uint8_t *)page2_text1, 32, 0x80);
	eeprom_read_buffer( (uint8_t *)page2_text2, 32, 0xA0);
	eeprom_read_buffer( (uint8_t *)page3_text1, 32, 0xC0);
	eeprom_read_buffer( (uint8_t *)page3_text2, 32, 0xE0);
	eeprom_read_buffer( (uint8_t *)page4_text1, 32, 0x100);
	eeprom_read_buffer( (uint8_t *)page4_text2, 32, 0x120);
	eeprom_read_buffer( (uint8_t *)page5_text1, 32, 0x140);
	eeprom_read_buffer( (uint8_t *)page5_text2, 32, 0x160);
}

void lcd_page_send(void){
		LcdClearScreen();
		switch(page){
			case 1:
				LcdCursor(0,(16 - strlen((char *)page1_text1)) / 2);
				LcdPrintStr((char *) page1_text1);
				LcdCursor(1,(16 - strlen((char *)page1_text2)) / 2);
				LcdPrintStr((char *) page1_text2);
				break;
			case 2:
				LcdCursor(0,(16 - strlen((char *)page2_text1)) / 2);
				LcdPrintStr((char *) page2_text1);
				LcdCursor(1,(16 - strlen((char *)page2_text2)) / 2);
				LcdPrintStr((char *) page2_text2);
				break;
			case 3:
				LcdCursor(0,(16 - strlen((char *)page3_text1)) / 2);
				LcdPrintStr((char *) page3_text1);
				LcdCursor(1,(16 - strlen((char *)page3_text2)) / 2);
				LcdPrintStr((char *) page3_text2);
				break;
			case 4:
				LcdCursor(0,(16 - strlen((char *)page4_text1)) / 2);
				LcdPrintStr((char *) page4_text1);
				LcdCursor(1,(16 - strlen((char *)page4_text2)) / 2);
				LcdPrintStr((char *) page4_text2);
				break;
			case 5:
				LcdCursor(0,(16 - strlen((char *)page5_text1)) / 2);
				LcdPrintStr((char *) page5_text1);
				LcdCursor(1,(16 - strlen((char *)page5_text2)) / 2);
				LcdPrintStr((char *) page5_text2);
				break;
		}
}