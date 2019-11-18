#include "ws2812b_modes.h"

volatile struct rgb_struct rgb;  //структура для получения цветов после перевода HSV->RGB

void Mode_1(void);
void Mode_2(void);
void Mode_3(void);

uint8_t Led_mode = 0;
uint8_t Led_mass_now[WS2812B_NUM_LEDS * 3] = {0};
uint8_t Led_mass[WS2812B_NUM_LEDS * 3] = {0};

/*
*Включение нужного режима
*/
void Turn_on_Led_mode(uint8_t mode){
	switch(mode)
	{
		case 0:   //перелевание радугой всех светодиодов
			Mode_1();
			break;
		case 1:   //перелевание разной радугой по группам
			Mode_2();
			break;
		case 2:   //бегающая радуга по всем светодиодам
			Mode_3();
			break;
			
	}
}
/*
*Режим 1
*/
void Mode_1(void){
	static int H = 0;
	H++;
	HSV(H, 255, 130);
	for(int i = 0; i < WS2812B_NUM_LEDS; i++){
		ws2812b_set(i, rgb.r, rgb.g, rgb.b);
	}
	while(!ws2812b_is_ready());
	ws2812b_send();
	if(H >= 360) H = 0;
	osDelay(15);
}

/*
*Доп функция для режима 2
*/
void Mode_2_buffer_fill(int H1, int H2, int H3, int H4, int V){  //функция заполнения массива для групп светодиодов
	int i = 0;
	for( i = 0; i < 2; i++){    //в каждую группу загружаем свой цвет
			HSV(H1, 255, V);
			ws2812b_set(i, rgb.r, rgb.g, rgb.b);
			taskYIELD();
		}
		for( i = 2; i < 5; i++){
			HSV(H2, 255, V);
			ws2812b_set(i, rgb.r, rgb.g, rgb.b);
			taskYIELD();
		}
		for( i = 5; i < 7; i++){
			HSV(H3, 255, V);
			ws2812b_set(i, rgb.r, rgb.g, rgb.b);
			taskYIELD();
		}
		for( i = 7; i < 9; i++){
			HSV(H4, 255, V);
			ws2812b_set(i, rgb.r, rgb.g, rgb.b);
			taskYIELD();
		}
}

/*
*Режим 2
*/
void Mode_2(void){
	int H1 = (rand() % 360);  //для первых группы    (2 св)
	int H2 = (rand() % 360);  //для второй группы    (3 св)
	int H3 = (rand() % 360);  //для третьей группы   (2 св)
	int H4 = (rand() % 360);  //для четвертой группы (2 св)
	srand(rand());
	taskYIELD();
	//в каждом цикле зажечь-погасить новые цвета для каждой группы,
	//получаемые рандомом
	for( int V = 0; V <= 180; V++){               //зажигаем и гасим светожиоды 
		Mode_2_buffer_fill(H1, H2, H3, H4, V);      //с выбранным значением Н для
		while(!ws2812b_is_ready());                 //каждой группы
		ws2812b_send();
		osDelay(10);
		taskYIELD();
	}
	for( int V = 180; V >= 0; V--){
		Mode_2_buffer_fill(H1, H2, H3, H4, V);
		while(!ws2812b_is_ready());
		ws2812b_send();
		osDelay(10);
		taskYIELD();
	}
}

uint8_t led_array_1[WS2812B_NUM_LEDS*3];                //массив для третьего режима

/*
*Доп функция для режима 3
*/
void Shift_led(void){                                   //ф-ция сдвига эл-ов массива в третьем режиме
	int x_r = led_array_1[0];                             //запоманаем первый цвет
	int x_g = led_array_1[1];
	int x_b = led_array_1[2];
	for(int i = 0; i < (WS2812B_NUM_LEDS*3 - 3) ; i += 3){//сдвигаем все на посицию назад
		led_array_1[i] = led_array_1[i + 3];
		led_array_1[i + 1] = led_array_1[i + 3 + 1];
		led_array_1[i + 2] = led_array_1[i + 3 + 2];
		taskYIELD();
	}
	led_array_1[WS2812B_NUM_LEDS * 3 - 3] = x_r;          //первый цвет теперь становится последним
	led_array_1[WS2812B_NUM_LEDS * 3 - 2] = x_g;
	led_array_1[WS2812B_NUM_LEDS * 3 - 1] = x_b;
}

/*
*Режим 3
*/
void Mode_3(void){
	int H = 0;
	static int flag = 1;                           //на первом прогоне будет 
	if(flag){                                      //заполняться массив с 
		for( H = 0; H <= 360; H += 40){              //радугой
			HSV(H, 255, 130);
			ws2812b_set(H / 40, rgb.r, rgb.g, rgb.b);  //далее этот массив будет 
			led_array_1[(H / 40) * 3] = rgb.r;         //цвигаться по светодиодам
			led_array_1[(H / 40) * 3 + 1] = rgb.g;
			led_array_1[(H / 40) * 3 + 2] = rgb.b;
			taskYIELD();
		}
		flag = 0;
	}
	for(int i = 0; i < WS2812B_NUM_LEDS; i++){     //на каждой новой итерации делаем сдвиг эл-ов
		while(!ws2812b_is_ready());                  //массива на одну возицию
		ws2812b_send();
		Shift_led(); //ф-ция сдвига
		taskYIELD();
		for(int j = 0; j < WS2812B_NUM_LEDS; j++){
			ws2812b_set(j, led_array_1[j*3], led_array_1[j*3 + 1], led_array_1[j*3 + 2]);
			taskYIELD();
		}
		osDelay(300);
	}
}
