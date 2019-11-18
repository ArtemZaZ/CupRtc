#include "freertos.h"

void StartRGBws2812bTask(void const * argument);
void StartButtonTask(void const * argument);
void StartLCDTask(void const * argument);
void ChangeLedMode(void const * argument);

extern uint8_t Led_mode;

void lcd_buffer_filling(void);

uint8_t page1_text1[16] = {0};
uint8_t page1_text2[16] = {0};
uint8_t page2_text1[16] = {0};
uint8_t page2_text2[16] = {0};
uint8_t page3_text1[16] = {0};
uint8_t page3_text2[16] = {0};
uint8_t page4_text1[16] = {0};
uint8_t page4_text2[16] = {0};

void freertos_init(void){
	lcd_buffer_filling();
	
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
	int page = 1;
	while(1) {
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
		}
		page++;
		if(page > 4) page = 1;
		osDelay(10000);
	}
}

void StartButtonTask(void const * argument)
{
	while(1){
		if(AntiContactBounce(GPIOA, GPIO_Pin_1)){
			int delay = 0;
			while(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) && delay < 1500){
				delay+=100;
				osDelay(100);
			}
			
			if(delay >= 1500){
				uint16_t num_of_frame = 0;
				eeprom_read_buffer( (uint8_t *)&num_of_frame, 32, 0x00);
				play_message_from_eeprom(SPEEX_ADDRESS, num_of_frame);
			}
			else{
				Led_mode++;
				if(Led_mode > 2) Led_mode = 0;
			}
		}
		osDelay(10);
	}
}

void StartRGBws2812bTask(void const * argument) 
{
	while(1){
		Turn_on_Led_mode(Led_mode);
	}
}

void lcd_buffer_filling(void){
	eeprom_read_buffer( (uint8_t *)page1_text1, 32, 0x20);
	eeprom_read_buffer( (uint8_t *)page1_text2, 32, 0x40);
	eeprom_read_buffer( (uint8_t *)page2_text1, 32, 0x60);
	eeprom_read_buffer( (uint8_t *)page2_text2, 32, 0x80);
	eeprom_read_buffer( (uint8_t *)page3_text1, 32, 0xA0);
	eeprom_read_buffer( (uint8_t *)page3_text2, 32, 0xC0);
	eeprom_read_buffer( (uint8_t *)page4_text1, 32, 0xE0);
	eeprom_read_buffer( (uint8_t *)page4_text2, 32, 0x100);
}
