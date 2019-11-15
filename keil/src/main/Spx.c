#include "Spx.h"

#define FRAME_SIZE             160
#define ENCODED_FRAME_SIZE      20

extern xSemaphoreHandle led;
extern xSemaphoreHandle lcd;

volatile int16_t OUT_Buffer[2][FRAME_SIZE];
volatile int16_t *outBuffer;

volatile uint8_t Start_Decoding = 0;
SpeexBits bits;
void *enc_state, *dec_state;
int quality = 4, complexity=1, vbr=0, enh=1;

void Speex_Init(void)
{
	speex_bits_init(&bits);
	dec_state = speex_decoder_init(&speex_nb_mode);
	speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);
	outBuffer = OUT_Buffer[0];
	spi_init();
}

void play_message(unsigned char const *array, uint16_t frame_number)
{
	char input_bytes[ENCODED_FRAME_SIZE];
	volatile uint16_t NB_Frames=0;
	TIM_Cmd(TIM2, ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);
	int i;
	uint16_t sample_index = 0;
	for(i=0;i<ENCODED_FRAME_SIZE; i++)
	{
		input_bytes[i] = array[sample_index];
		sample_index++;
	}
	speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
	speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[0]);
	NB_Frames++;
	for(i=0;i<ENCODED_FRAME_SIZE; i++)
	{
		input_bytes[i] = array[sample_index];
		sample_index++;
	} 
	speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
	speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[1]);
	NB_Frames++;
	while(NB_Frames < frame_number)
	{
		if(Start_Decoding == 1)
		{
			for(i=0;i<ENCODED_FRAME_SIZE; i++)
			{
				input_bytes[i] = array[sample_index];
				sample_index++;
			}
			speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
			speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[0]);
			
			Start_Decoding = 0;
			NB_Frames++;
		}
		if(Start_Decoding == 2)
		{
			for(i=0;i<ENCODED_FRAME_SIZE; i++)
			{
				input_bytes[i] = array[sample_index];
				sample_index++;
			}
			speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
			speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[1]);
			
			Start_Decoding = 0;
			NB_Frames++;
		}
		taskYIELD();
	}
	
	TIM_Cmd(TIM2, DISABLE);
	NVIC_DisableIRQ(TIM2_IRQn);
	sample_index = 0;
	NB_Frames = 0;
	outBuffer = OUT_Buffer[0];
}

extern TaskHandle_t xHandleLED;
extern TaskHandle_t xHandleLCD;

void play_message_from_eeprom(uint16_t address, uint16_t frame_number){
	volatile uint16_t NB_Frames=0;
	char input_bytes[ENCODED_FRAME_SIZE];
	eeprom_read_buffer((uint8_t *)input_bytes, 20, address);
	address += 20;
	speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
	speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[0]);
	
	eeprom_read_buffer((uint8_t *)input_bytes, 20, address);
	address += 20;
	speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
	speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[1]);
	NB_Frames+=2;
	
	TIM_Cmd(TIM2, ENABLE);
	NVIC_EnableIRQ(TIM2_IRQn);

	while(NB_Frames < frame_number)
	{
		if(Start_Decoding == 1)
		{
			eeprom_read_buffer((uint8_t *)input_bytes, 20, address);
			address += 20;
			speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
			speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[0]);
			Start_Decoding = 0;
			NB_Frames++;
		}
		if(Start_Decoding == 2)
		{
			eeprom_read_buffer((uint8_t *)input_bytes, 20, address);
			address += 20;
			speex_bits_read_from(&bits, input_bytes, ENCODED_FRAME_SIZE);
			speex_decode_int(dec_state, &bits, (spx_int16_t*)OUT_Buffer[1]);
			Start_Decoding = 0;
			NB_Frames++;
		}
		taskYIELD();
	}
	taskYIELD();
	NVIC_DisableIRQ(TIM2_IRQn);
	TIM_Cmd(TIM2, DISABLE);
	NB_Frames = 0;
	outBuffer = OUT_Buffer[0];
	taskYIELD();
}
