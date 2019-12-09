#include "Lcd.h"

#define			LCM_PIN_RS			GPIO_Pin_10         //PA10
#define			LCM_PIN_EN			GPIO_Pin_11         //PA11
#define			LCM_PIN_D7			GPIO_Pin_4          //PB4
#define			LCM_PIN_D6			GPIO_Pin_3          //PB3
#define			LCM_PIN_D5			GPIO_Pin_15         //PA15
#define			LCM_PIN_D4			GPIO_Pin_12         //PA12

//набор дефайнов для более быстрого управления ногами
#define			LCM_PIN_MASK_A	(LCM_PIN_RS | LCM_PIN_EN | LCM_PIN_D5 | LCM_PIN_D4)
#define			LCM_PIN_MASK_B	(LCM_PIN_D7 | LCM_PIN_D6)
#define			LCM_OUT_A				GPIOA->ODR
#define			LCM_OUT_B				GPIOB->ODR

const char rus[] = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя";
											//А    Б     В    Г    Д    Е    Ё
const uint8_t RUS[66]={0x41,0xA0,0x42,0xA1,0xE0,0x45,0xA2,
											//Ж    З    И    Й    К    Л    М
												0xA3,0xA4,0xA5,0xA6,0x4B,0xA7,0x4D,
											//Н    О    П    Р    С    Т    У
												0x48,0x4F,0xA8,0x50,0x43,0x54,0xA9,
											//Ф    Х    Ц    Ч    Ш    Щ    Ъ
												0xAA,0x58,0xE1,0xAB,0xAC,0xE2,0xAD,
											//Ы    Ь    Э    Ю    Я
												0xAE,0x62,0xAF,0xB0,0xB1,
											//а     б    в    г    д    е    ё
												0x61,0xB2,0xB3,0xB4,0xE3,0x65,0xB5,
											//ж    з    и    й    к    л    м
												0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,
											//н    о    п    р    с    т    у
												0xBD,0x6F,0xBE,0x70,0x63,0xBF,0x79,
											//ф    х    ц    ч    ш    щ    ъ
												0xE4,0x78,0xE5,0xC0,0xC1,0xE6,0xC2,
											//ы    ь    э    ю    я
												0xC3,0xC4,0xC5,0xC6,0xC7};

/*
*Импульс на ноге Е для записи дисплеем заданных параметров
*/
void PulseLCD(void)
{
	LCM_OUT_A |= LCM_PIN_EN;     //ставим бит Е и ждем 2мкс
	delay_us(2);
	LCM_OUT_A &= (~LCM_PIN_EN);  //потом снимаем
}

/*
*Отсылка байта в дисплей
*/
void LcdSendByte(char ByteToSend, int IsData)
{
	LCM_OUT_A &= ~(LCM_PIN_MASK_A);               //выкавыриваем старший полубайт из команды
	LCM_OUT_B &= ~(LCM_PIN_MASK_B);               //и расставляем биты на нужных ножках
	LCM_OUT_A |= ((ByteToSend & 0x10)) << 8;
	LCM_OUT_A |= ((ByteToSend & 0x20)) << 10;
	LCM_OUT_B |= ((ByteToSend & 0xC0)) >> 3;
	if (IsData == 1){           //если команда то надо установить бит RS
		LCM_OUT_A |= LCM_PIN_RS;
		delay_us(4);
	}
	else{
		LCM_OUT_A &= ~LCM_PIN_RS; //если информация то наоборот снять
		delay_us(4);
	}
	PulseLCD();                 //отправляем старший полубайт
	delay_us(5);
	LCM_OUT_A &= ~(LCM_PIN_MASK_A);           //выкавыриваем младший полубайт
	LCM_OUT_B &= ~(LCM_PIN_MASK_B);           //также расставлем биты по своим
	LCM_OUT_A |= ((ByteToSend & 0x1)) << 12;  //позициям
	LCM_OUT_A |= ((ByteToSend & 0x2)) << 14;
	LCM_OUT_B |= ((ByteToSend & 0xC)) << 1;
	if (IsData == 1){           //если команда то надо установить бит RS
		LCM_OUT_A |= LCM_PIN_RS;
		delay_us(4);
	}
	else{
		LCM_OUT_A &= ~LCM_PIN_RS; //если информация то наоборот снять
		delay_us(4);
	}
	PulseLCD();                     //отправляем младший полубайт
	osDelay(5);
	
	LCM_OUT_A &= ~(LCM_PIN_MASK_A); //зачищаем используемые ноги
	LCM_OUT_B &= ~(LCM_PIN_MASK_B); //на обоих портах
}

/*
*Установка позиции курсора
*/
void LcdCursor(char Row, char Col)
{
	char address;
	if (Row == 0)
		address = 0;     //определяем необходимый адрес
	else               //и отправляем команды с этим адресом
		address = 0x40;
	address |= Col;
	LcdSendByte(0x80 | address, 0);
}
 
/*
*Очистка дисплея
*/
void LcdClearScreen(void)
{
	LcdSendByte(0x01, 0); //команда с одной единичкой
	osDelay(3);
}

/*
*ВНИМАНИЕ КОСТЫЛИ
*Две функции ниже являются локальными и нужны только для инициализации дисплея
*они являются полными копиями функций SendByte() и PulseLCD() за исключением исполнения
*задержек, так как для успешной работы дисплея в системе freertos задержки должны быть
*freertos-овскими, а при инициализации дисплея freertos еще не запущен. 
*/
void PulseLCD_1(void)
{
	LCM_OUT_A |= LCM_PIN_EN;     //ставим бит Е и ждем 2мкс
	delay_us(2);
	LCM_OUT_A &= (~LCM_PIN_EN);  //потом снимаем
}
void SendByte_1(char ByteToSend, int IsData)
{
	LCM_OUT_A &= ~(LCM_PIN_MASK_A);               //выкавыриваем старший полубайт из команды
	LCM_OUT_B &= ~(LCM_PIN_MASK_B);               //и расставляем биты на нужных ножках
	LCM_OUT_A |= ((ByteToSend & 0x10)) << 8;
	LCM_OUT_A |= ((ByteToSend & 0x20)) << 10;
	LCM_OUT_B |= ((ByteToSend & 0xC0)) >> 3;
	if (IsData == 1){           //если команда то надо установить бит RS
		LCM_OUT_A |= LCM_PIN_RS;
		delay_us(4);
	}
	else{
		LCM_OUT_A &= ~LCM_PIN_RS; //если информация то наоборот снять
		delay_us(4);
	}
	PulseLCD_1();                 //отправляем старший полубайт
	delay_us(5);
	
	LCM_OUT_A &= ~(LCM_PIN_MASK_A);           //выкавыриваем младший полубайт
	LCM_OUT_B &= ~(LCM_PIN_MASK_B);           //также расставлем биты по своим
	LCM_OUT_A |= ((ByteToSend & 0x1)) << 12;  //позициям
	LCM_OUT_A |= ((ByteToSend & 0x2)) << 14;
	LCM_OUT_B |= ((ByteToSend & 0xC)) << 1;
	if (IsData == 1){           //если команда то надо установить бит RS
		LCM_OUT_A |= LCM_PIN_RS;
		delay_us(4);
	}
	else{
		LCM_OUT_A &= ~LCM_PIN_RS; //если информация то наоборот снять
		delay_us(4);
	}
	PulseLCD_1();                     //отправляем младший полубайт
	delay_ms(5);
	
	LCM_OUT_A &= ~(LCM_PIN_MASK_A); //зачищаем используемые ноги
	LCM_OUT_B &= ~(LCM_PIN_MASK_B); //на обоих портах
}

/*
*Инициализация дисплея
*/
void LCD_Init(void)
{
	Delay_Init();
	//включаем всю перефирию
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//отключаем лишнее, чтобы ничего не мешалось
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	
	GPIO_InitTypeDef LcdWriteA;
	GPIO_InitTypeDef LcdWriteB;
	
	//настройка на порту А
	LcdWriteA.GPIO_Pin = (LCM_PIN_EN | LCM_PIN_RS | LCM_PIN_D5 | LCM_PIN_D4);
	LcdWriteA.GPIO_Mode = GPIO_Mode_Out_PP;
	LcdWriteA.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &LcdWriteA);
	
	//настройка на порту В
	LcdWriteB.GPIO_Pin = (LCM_PIN_D6 | LCM_PIN_D7);
	LcdWriteB.GPIO_Mode = GPIO_Mode_Out_PP;
	LcdWriteB.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &LcdWriteB);
	
	delay_ms(20);              //при подаче питания рекомендовано выждать 20мс
	LCM_OUT_A &= ~(LCM_PIN_MASK_A);
	LCM_OUT_B &= ~(LCM_PIN_MASK_B);
	
	LCM_OUT_A &= ~LCM_PIN_RS;  //отправка 0011 (8битный интерфейс)
	LCM_OUT_A &= ~LCM_PIN_EN;
	LCM_OUT_A |= 0x9000;
	LCM_OUT_B |= 0x0000;
	PulseLCD_1();
	delay_ms(5);
	PulseLCD_1();
	delay_ms(5);
	LCM_OUT_A &= ~(LCM_PIN_MASK_A);
	LCM_OUT_B &= ~(LCM_PIN_MASK_B);
	
	LCM_OUT_A |= 0x8000;       //отправка 0010 (4битный интерфейс), несколько раз
	LCM_OUT_B |= 0x000;
	PulseLCD_1();
	delay_us(10);
	
	LCM_OUT_A &= ~(LCM_PIN_MASK_A);
	LCM_OUT_B &= ~(LCM_PIN_MASK_B);
	LCM_OUT_A |= 0x8000;       //отправка 0010 (4битный интерфейс), несколько раз
	LCM_OUT_B |= 0x000;
	PulseLCD_1();
	delay_ms(5);
	LCM_OUT_A &= ~(LCM_PIN_MASK_A);
	LCM_OUT_B &= ~(LCM_PIN_MASK_B);
	
	SendByte_1(0x28, 0);      //101000 еще раз про интерфес + две строки + размер шрифта
	SendByte_1(0x06, 0);      //110 вывод символов слева-направо, инкремент указателя памяти + запрет сдвига экрана
	SendByte_1(0x14, 0);      //10100 сдвигаем курсор, а не экран + сдвигаем курсор влево
	SendByte_1(0x0C, 0);      //1100 экран включен + курсор выключен + курсор не мигает
}
 
/*
*Печать строки
*/
void LcdPrintStr(char *Text)
{	
	char *c = Text;
	while ((c != 0) && (*c != 0) && (*c != 0x0A))    //перебираем символы из текста
	{
		char y[2];
		strncpy( y, c, 2);  //проверяем не русский ли символ
		if(y){
			if(strstr(rus, y)){          //если русский, то кодировку берем из массива
				LcdSendByte(RUS[abs(rus - strstr(rus, y)) / 2], 1);
				c += 2; //*кейл кодирует кириллицу двумя байтами
			}
			else 
			{
				LcdSendByte(*c, 1);           //если английский, то подойдет кодировка кейла
				c+=1;
			}
		}
	}
}

/*
*Печать переменной
*/
void LcdPrintVar(int y)
{	
	char str[8];
	sprintf (str, "%i", y);
	LcdPrintStr(str);
}

/*
*Зачистка куска дисплея
*/
void LcdCleaning (int row, int col, int number)
{
	LcdCursor(row, col);
	for(int i = 0; i < number; i++){
		LcdSendByte(' ', 1);
	}
}
