#include "LCD.h"

//Массивы ниже необходимы для успешной печати на русском
//Подробнее в функции LСD_Print_Str()
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
*Настройка всех выводов и Инициализация дисплея
*/
void LCD_Init(void)
{
	Delay_Init();  //требуется библиотека с задержками
	
	//НАСТРОЙКА ВСЕХ ВЫВОДОВ
	LCD_RCC_APBxPeriphClockCmd(LCD_RCC_APBxPeriph_AFIO, ENABLE);
	//Настрйока вывода DB4
	LCD_RCC_APBxPeriphClockCmd(LCD_DB4_RCC_APBxPeriph_GPIOx, ENABLE);
	GPIO_InitTypeDef Db4_Struct;
	Db4_Struct.GPIO_Pin = LCD_DB4_GPIO_Pin_x;
	Db4_Struct.GPIO_Mode = LCD_GPIO_Mode;
	Db4_Struct.GPIO_Speed = LCD_GPIO_Speed;
	GPIO_Init(LCD_DB5_GPIOx, &Db4_Struct);
	//Настрйока вывода DB5
	LCD_RCC_APBxPeriphClockCmd(LCD_DB5_RCC_APBxPeriph_GPIOx, ENABLE);
	GPIO_InitTypeDef Db5_Struct;
	Db5_Struct.GPIO_Pin = LCD_DB5_GPIO_Pin_x;
	Db5_Struct.GPIO_Mode = LCD_GPIO_Mode;
	Db5_Struct.GPIO_Speed = LCD_GPIO_Speed;
	GPIO_Init(LCD_DB5_GPIOx, &Db5_Struct);
	//Настрйока вывода DB6
	LCD_RCC_APBxPeriphClockCmd(LCD_DB6_RCC_APBxPeriph_GPIOx, ENABLE);
	GPIO_InitTypeDef Db6_Struct;
	Db6_Struct.GPIO_Pin = LCD_DB6_GPIO_Pin_x;
	Db6_Struct.GPIO_Mode = LCD_GPIO_Mode;
	Db6_Struct.GPIO_Speed = LCD_GPIO_Speed;
	GPIO_Init(LCD_DB6_GPIOx, &Db6_Struct);
	//Настрйока вывода DB7
	LCD_RCC_APBxPeriphClockCmd(LCD_DB7_RCC_APBxPeriph_GPIOx, ENABLE);
	GPIO_InitTypeDef Db7_Struct;
	Db7_Struct.GPIO_Pin = LCD_DB7_GPIO_Pin_x;
	Db7_Struct.GPIO_Mode = LCD_GPIO_Mode;
	Db7_Struct.GPIO_Speed = LCD_GPIO_Speed;
	GPIO_Init(LCD_DB7_GPIOx, &Db7_Struct);
	//Настрйока вывода RS
	LCD_RCC_APBxPeriphClockCmd(LCD_RS_RCC_APBxPeriph_GPIOx, ENABLE);
	GPIO_InitTypeDef Rs_Struct;
	Rs_Struct.GPIO_Pin = LCD_RS_GPIO_Pin_x;
	Rs_Struct.GPIO_Mode = LCD_GPIO_Mode;
	Rs_Struct.GPIO_Speed = LCD_GPIO_Speed;
	GPIO_Init(LCD_RS_GPIOx, &Rs_Struct);
	//Настрйока вывода EN
	LCD_RCC_APBxPeriphClockCmd(LCD_EN_RCC_APBxPeriph_GPIOx, ENABLE);
	GPIO_InitTypeDef En_Struct;
	En_Struct.GPIO_Pin = LCD_EN_GPIO_Pin_x;
	En_Struct.GPIO_Mode = LCD_GPIO_Mode;
	En_Struct.GPIO_Speed = LCD_GPIO_Speed;
	GPIO_Init(LCD_EN_GPIOx, &En_Struct);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //отключаем лишнее, чтобы ничего не мешалось
	delay_ms(20);  //при подаче питания рекомендовано выждать 20мс
	
	LCD_GPIO_Reset_All();
	LCD_GPIO_Set_Half_Byte(0x03);  //порядок согласно даташиту. Сначала указываем что 8-ми битный интерфейс
	LCD_Pulse_EN();  //дергаем отправкой
	delay_us(50);;  //выжидаем
	LCD_GPIO_Reset_All();  //зачищаем выводы
	LCD_GPIO_Set_Half_Byte(0x02);  //указываем что 4-ех битный интерфейс
	LCD_Pulse_EN();  //дергаем отправкой
	delay_us(50);  //выжидаем
	LCD_GPIO_Reset_All(); //зачищаем выводы
	
	//далее отправляются команды настройки. Сначала отправляется старшая половина, потом младшая.
	//таблица с командами есть в даташите
	LCD_Send_Byte(0x2C, 0);  //4ех битный интерфейс, 2 строки, 5х8 точек
	LCD_Send_Byte(0x0C, 0);  //вкл дисплей, выкл курсор, выкл мигание курсора
	LCD_Send_Byte(0x14, 0);  //двигается курсор а не экран, двигается курсор вправо
	LCD_Send_Byte(0x04, 0);  //курсор вправо, сдвиг не сопровождается сдвигом символов
	LCD_Clear_Screen();  //очистка экрана
}

/*
*Выставление на шине обмена данных с дисплеем 4ех битов данных
*/
void LCD_GPIO_Set_Half_Byte(uint8_t byte){
	if((byte >> 0) & 0x01)  //постепенно расставляем биты по шине данных
		GPIO_SetBits(LCD_DB4_GPIOx, LCD_DB4_GPIO_Pin_x);
	if((byte >> 1) & 0x01)
		GPIO_SetBits(LCD_DB5_GPIOx, LCD_DB5_GPIO_Pin_x);
	if((byte >> 2) & 0x01)
		GPIO_SetBits(LCD_DB6_GPIOx, LCD_DB6_GPIO_Pin_x);
	if((byte >> 3) & 0x01)
		GPIO_SetBits(LCD_DB7_GPIOx, LCD_DB7_GPIO_Pin_x);
}

/*
*Сброс всей шины данных и информационных ног (отправки и команда/данные)
*/
void LCD_GPIO_Reset_All(void){
	GPIO_ResetBits(LCD_DB4_GPIOx, LCD_DB4_GPIO_Pin_x);
	GPIO_ResetBits(LCD_DB5_GPIOx, LCD_DB5_GPIO_Pin_x);
	GPIO_ResetBits(LCD_DB6_GPIOx, LCD_DB6_GPIO_Pin_x);
	GPIO_ResetBits(LCD_DB7_GPIOx, LCD_DB7_GPIO_Pin_x);
	GPIO_ResetBits(LCD_RS_GPIOx, LCD_RS_GPIO_Pin_x);
	GPIO_ResetBits(LCD_EN_GPIOx, LCD_EN_GPIO_Pin_x);
}

/*
*Команда подачи сигнала чтения/записи на выводе EN
*/
void LCD_Pulse_EN(void){
	GPIO_SetBits(LCD_EN_GPIOx, LCD_EN_GPIO_Pin_x); //выставляем EN и чуть ждем
	delay_us(2);
	GPIO_ResetBits(LCD_EN_GPIOx, LCD_EN_GPIO_Pin_x); //снимаем EN
}

/*
*Отправка байта данных/команды на дисплей
*/
void LCD_Send_Byte(char ByteToSend, uint8_t IsData)
{
	LCD_GPIO_Reset_All();
	LCD_GPIO_Set_Half_Byte(ByteToSend >> 4);  //выкавыриваем старший полубайт из команды и расставляем биты на нужных ножках
	
	if (IsData == 1)  //если данные то надо установить бит RS
		GPIO_SetBits(LCD_RS_GPIOx, LCD_RS_GPIO_Pin_x);
	else
		GPIO_ResetBits(LCD_RS_GPIOx, LCD_RS_GPIO_Pin_x);  //если команда то наоборот снять
	
	LCD_Pulse_EN();  //отправляем старший полубайт
	LCD_GPIO_Reset_All();  //зачищаем
	LCD_GPIO_Set_Half_Byte(ByteToSend & 0x0F);  //выкавыриваем младший полубайт и также расставлем биты по своим позициям
	
	if (IsData == 1)  //если данные то надо установить бит RS
		GPIO_SetBits(LCD_RS_GPIOx, LCD_RS_GPIO_Pin_x);
	else
		GPIO_ResetBits(LCD_RS_GPIOx, LCD_RS_GPIO_Pin_x);  //если команда то наоборот снять
	
	LCD_Pulse_EN();  //отправляем младший полубайт
	delay_us(50);
	
	LCD_GPIO_Reset_All();  //зачищаем
}

/*
*Очистка дисплея
*/
void LCD_Clear_Screen(void)
{
	LCD_Send_Byte(0x01, 0); //команда с одной единичкой
	delay_ms(2);
}

/*
*Зачистка куска дисплея
*/
void LCD_Cleaning (int row, int col, int number)
{
	LCD_Set_Cursor(row, col);  //устанавливаем курсор в нужную точку
	for(int i = 0; i < number; i++){  //и зачищаем необходимое кол-во
		LCD_Send_Byte(' ', 1);
	}
}

/*
*Установка позиции курсора
*/
void LCD_Set_Cursor(char Row, char Col)
{
	char address;
	if (Row == 0)
		address = 0;  //определяем необходимый адрес
	else  //и отправляем команды с этим адресом
		address = 0x40;
	address |= Col;
	LCD_Send_Byte(0x80 | address, 0);
}

/*
*Печать строки
*Здесь начинаются костыли. Возможно с такого бывалые прогеры захотят меня убить, но других идей у меня не было
*Кодировка кирилицы у кейла не совпадает с кодировкой кирилицы дисплея
*Поэтому пришлось городить переход от одной кодировки к другой через массив с кодами дисплея
*/
void LСD_Print_Str(char *Text)
{	
	char *c = Text;
	while ((c != 0) && (*c != 0) && (*c != 0x0A))  //перебираем символы из текста
	{
		char y[2];  //кирилица кодируется двумя байтами
		strncpy(y, c, 2);  //проверяем не русский ли символ
		if(y){
			if(strstr(rus, y)){  //если русский, то кодировку берем из массива
				LCD_Send_Byte(RUS[abs(rus - strstr(rus, y)) / 2], 1);
				c += 2;  //*кейл кодирует кириллицу двумя байтами
			}
			else 
			{
				LCD_Send_Byte(*c, 1);  //если английский, то подойдет кодировка кейла
				c+=1;
			}
		}
	}
}

/*
*Печать переменной
*/
void LCD_Print_Var(int var)
{	
	char str[8];
	sprintf (str, "%i", var);
	LСD_Print_Str(str);
}
