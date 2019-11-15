#include "ws2812b.h"
#ifdef USE_LED_MATRIX
//Передача лог. нуля 0.4мкс
#define WS2812B_0_VAL						(WS2812B_TIMER_AAR / 3)
//Передача лог. единицы 0.85мкс
#define WS2812B_1_VAL 					((WS2812B_TIMER_AAR / 3) * 2)
//Сигнал RET или RESET более 50мкс
#define WS2812B_TIMER_RET				(WS2812B_TIMER_AAR * 45)

#define GPIO_CRL_CNFx						GPIO_CRL_CNF6
#define GPIO_CRL_CNFx_1					GPIO_CRL_CNF6_1
#define GPIO_CRL_MODEx_1				GPIO_CRL_MODE6_1
#define GPIO_CRL_MODEx_0				GPIO_CRL_MODE6_0

#define TIM											TIM3
#define TIM_CCER_CCxE						TIM_CCER_CC1E
#define TIM_CCER_CCxP						TIM_CCER_CC1P

#define CCMRx										CCMR1
#define TIM_CCMRy_OCxM					TIM_CCMR1_OC1M
#define TIM_CCMRy_OCxM_2				TIM_CCMR1_OC1M_2
#define TIM_CCMRy_OCxM_1				TIM_CCMR1_OC1M_1
#define TIM_CCMRy_OCxPE					TIM_CCMR1_OC1PE
#define TIM_DIER_CCxDE					TIM_DIER_CC1DE
#define CCRx											CCR1

#define DMA1_Channelx							DMA1_Channel6
#define DMA1_Channelx_IRQn				DMA1_Channel6_IRQn
#define DMA1_Channelx_IRQHandler	DMA1_Channel6_IRQHandler

#define DMA_CCRx_EN							DMA_CCR6_EN
#define DMA_CCRx_TCIE						DMA_CCR6_TCIE

#define DMA_IFCR_CTEIFx					DMA_IFCR_CTEIF6
#define DMA_IFCR_CHTIFx					DMA_IFCR_CHTIF6
#define DMA_IFCR_CTCIFx					DMA_IFCR_CTCIF6
#define DMA_IFCR_CGIFx					DMA_IFCR_CGIF6

#define DATA_LEN ((WS2812B_NUM_LEDS * 24) + 2)

static uint8_t led_array[DATA_LEN];
static int flag_rdy = 0;
extern volatile struct rgb_struct rgb;
static void bus_retcode(void);

void ws2812b_init(void)
{
	flag_rdy = 0;
	//Разрешаем такирование переферии
	RCC->APB2ENR 	|= RCC_APB2ENR_IOPAEN;
	RCC->APB1ENR	|= RCC_APB1ENR_TIM3EN;
	RCC->AHBENR		|= RCC_AHBENR_DMA1EN;
	
	/********* Настраиваем PA6 *********/
	//PA6 freq=10Mhz, AF output Push-pull
	GPIOA->CRL &= ~(GPIO_CRL_CNFx);
	GPIOA->CRL |= GPIO_CRL_CNFx_1 | GPIO_CRL_MODEx_1 | GPIO_CRL_MODEx_0;
	/********* Настойка таймера TIM3 *********/
	//Разрешаем таймеру управлять выводом PA6
	TIM->CCER |= TIM_CCER_CCxE;    //Разрешаем
	TIM->CCER &= ~(TIM_CCER_CCxP);
	TIM->CCMRx &= ~(TIM_CCMRy_OCxM); //сбрасываем все биты OCxM
	
	//устанавливаем выход в неактивное состояние
	TIM->CCMRx |= TIM_CCMRy_OCxM_2; 
	TIM->CCMRx &= ~(TIM_CCMRy_OCxM_2);
	
	TIM->CCMRx |= TIM_CCMRy_OCxM_2 | TIM_CCMRy_OCxM_1 | TIM_CCMRy_OCxPE; //режим ШИМ-а
	TIM->CR1 |= TIM_CR1_ARPE;    //Регистры таймера с буферизацией
	TIM->DIER |= TIM_DIER_CCxDE; //Разрешить запрос DMA
	
	//Настраиваем канал DMA
	DMA1_Channelx->CPAR = (uint32_t)(&TIM->CCRx); //Куда пишем
	DMA1_Channelx->CMAR = (uint32_t)(led_array); //откуда берем
	DMA1_Channelx->CCR = DMA_CCR6_PSIZE_0 //регистр переферии 16 бит
			| DMA_CCR6_MINC //режим инкремента указателя памяти
			| DMA_CCR6_DIR; //напревление передачи из памяти в переферию
	
	//Разрешаем обработку прерываний
	NVIC_EnableIRQ(TIM3_IRQn); //от таймера
	NVIC_EnableIRQ(DMA1_Channelx_IRQn); //от DMA
	
	ws2812b_buff_clear();
	bus_retcode(); //сбрасываем шину
}

void ws2812b_buff_clear(void)
{
	for(int i = 0; i < DATA_LEN; i++)
		led_array[i] = WS2812B_0_VAL;
	led_array[DATA_LEN-2] = 0;
	led_array[DATA_LEN-1] = 0;
}

int ws2812b_set(int pixn, uint8_t r, uint8_t g, uint8_t b)
{
	int offset = pixn*24;
	int i;
	uint8_t tmp;
	
	if(pixn > (WS2812B_NUM_LEDS - 1))
		return 1;
	
	//g component
	tmp = g;
	for(i=0; i<8; i++)
	{
		if(tmp & 0x80)
			led_array[offset + i] = WS2812B_1_VAL;
		else
			led_array[offset + i] = WS2812B_0_VAL;
		tmp<<=1;
	}
	
	//r component
	tmp = r;
	for(i=0; i<8; i++)
	{
		if(tmp & 0x80)
			led_array[offset + i + 8] = WS2812B_1_VAL;
		else
			led_array[offset + i + 8] = WS2812B_0_VAL;
		tmp<<=1;
	}
	
	//b component
	tmp = b;
	for(i=0; i<8; i++)
	{
		if(tmp & 0x80)
			led_array[offset + i + 16] = WS2812B_1_VAL;
		else
			led_array[offset + i + 16] = WS2812B_0_VAL;
		tmp<<=1;
	}
	
	return 0;
}

int ws2812b_send(void)
{
	if(flag_rdy) //Если сейчас ни чего не передается
	{
		//Устанавливаем флаг занятости интерфейса
		flag_rdy = 0;
		
		//Настраиваем передачу данных
		DMA1_Channelx->CCR &= ~(DMA_CCR6_EN); //Отключаем канал DMA
		DMA1_Channelx->CNDTR = sizeof(led_array); //Устанавливаем количество данных
		
		//Таймер считает до WS2812B_TIMER_AAR, таким образом
		//при данной частоте тактирования таймера
		//получаем период ШИМ-сигнала, равный 1.25мкс
		TIM->ARR = WS2812B_TIMER_AAR;
		TIM->CCRx = 0x0000; //Устанавливаем ШИМ-регистр таймера в ноль
		TIM->CNT = 0; //Очищаем счетный регистр
		TIM->CR1 |= TIM_CR1_CEN; //Запускаем таймер
		//Так как значение ШИМ установили в ноль, 
		//то на шине будет установлен неактивный уровень
		//до момента запуска DMA  
		
		DMA1->IFCR = DMA_IFCR_CTEIFx | DMA_IFCR_CHTIFx 
			| DMA_IFCR_CTCIFx | DMA_IFCR_CGIFx; //Очищаем все флаги прерываний DMA
		
		DMA1_Channelx->CCR |= DMA_CCRx_TCIE; //прерывание завершения передачи
		
		//Включаем канал DMA, тем самым начинаем передачу данных
		DMA1_Channelx->CCR |= DMA_CCRx_EN; 
		return 0;
	}
	else
	{
		return 1;
	}
}

int ws2812b_is_ready(void)
{
	return flag_rdy;
}

static void bus_retcode(void)
{
	TIM->CR1 &= ~(TIM_CR1_CEN); //останавливаем таймер
	TIM->ARR = WS2812B_TIMER_RET; //Устанавливаем период немного больше 50мкс
	TIM->CNT = 0; //Очищаем счетный регистр
	TIM->CCRx = 0x0000; //значение ШИМ-а ноль
	TIM->SR &= ~(TIM_SR_UIF); //сбрасываем флаг прерывания
	TIM->DIER |= TIM_DIER_UIE; //прерывание по обновлению
	TIM->CR1 |= TIM_CR1_CEN; //Поехали считать!
}

//Прерывание от DMA
//Суда попадаем после завершения передачи данных
void DMA1_Channelx_IRQHandler(void)
{
	DMA1_Channelx->CCR &= ~(DMA_CCRx_EN); //Отключаем канал DMA

	DMA1->IFCR = DMA_IFCR_CTEIFx | DMA_IFCR_CHTIFx 
		| DMA_IFCR_CTCIFx | DMA_IFCR_CGIFx; //Сбрасываем все флаги прерываний

	//Так как последние 2 элемента массива равны нулю,
	//то сейчас предпоследнее значение уже загружено
	//в теневой регистр сравнения
	//и на шине установлено неактивное состояние.
	//Задача заключается в удержании шины в этом состоянии
	//в течение 50мкс или более
	//перед установкой флага готовности интерфейса.

	bus_retcode();
}


//прерывание от таймера
//Сюда попадаем после завершения формирования 
//сигнала RET шины ws2812b
void TIM3_IRQHandler(void)
{
	TIM->SR = 0; //Сбрасываем все флаги прерываний

	//Итак, мы завершили формирование сигнала RET на шине
	//и теперь можно сделать все завершающие операции 
	//и установить флаг готовности интерфейса к следующей
	//передаче данных.

	TIM->CR1 &= ~(TIM_CR1_CEN); //останавливаем таймер
	TIM->DIER &= ~(TIM_DIER_UIE); //запрещаем прерывание таймера

	flag_rdy = 1;
}
const uint8_t MASS[256] = {
  0, 1, 1, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4,
  4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6,
  6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8,
  8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 11, 11, 11,
  11, 11, 12, 12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15,
  15, 15, 16, 16, 16, 16, 17, 17, 17, 18, 18, 18, 19, 19, 19, 20,
  20, 20, 21, 21, 22, 22, 22, 23, 23, 24, 24, 25, 25, 25, 26, 26,
  27, 27, 28, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34, 35, 35,
  36, 36, 37, 38, 38, 39, 40, 40, 41, 42, 43, 43, 44, 45, 46, 47,
  48, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,
  63, 64, 65, 66, 68, 69, 70, 71, 73, 74, 75, 76, 78, 79, 81, 82,
  83, 85, 86, 88, 90, 91, 93, 94, 96, 98, 99, 101, 103, 105, 107, 109,
  110, 112, 114, 116, 118, 121, 123, 125, 127, 129, 132, 134, 136, 139, 141, 144,
  146, 149, 151, 154, 157, 159, 162, 165, 168, 171, 174, 177, 180, 183, 186, 190,
  193, 196, 200, 203, 207, 211, 214, 218, 222, 226, 230, 234, 238, 242, 248, 255,
};
 
void HSV(int H, int S, int V){
	V = MASS[V];
	S = 255 - MASS[255 - S];
	
	int Hi = (H / 60);
	int Vmin = ((255 - S) * V) >> 8;
	int Vinc = (((V - Vmin) * (H % 60)) / 60) + Vmin;
	int Vdec = (((V - Vmin) * (60 - (H % 60))) / 60) + Vmin;
	if(S == 0){
		rgb.r = V;
		rgb.g = V;
		rgb.b = V;
	}
	else{
		switch(Hi){
			case 0:
				rgb.r = V;
				rgb.g = Vinc;
				rgb.b = Vmin;
				break;
			case 1:
				rgb.r = Vdec;
				rgb.g = V;
				rgb.b = Vmin;
				break;
			case 2:
				rgb.r = Vmin;
				rgb.g = V;
				rgb.b = Vinc;
				break;
			case 3:
				rgb.r = Vmin;
				rgb.g = Vdec;
				rgb.b = V;
				break;
			case 4:
				rgb.r = Vinc;
				rgb.g = Vmin;
				rgb.b = V;
				break;
			case 5:
				rgb.r = V;
				rgb.g = Vmin;
				rgb.b = Vdec;
				break;
		}
		rgb.r = rgb.r & 0xFF;
		rgb.g = rgb.g & 0xFF;
		rgb.b = rgb.b & 0xFF;
	}
}

#endif
