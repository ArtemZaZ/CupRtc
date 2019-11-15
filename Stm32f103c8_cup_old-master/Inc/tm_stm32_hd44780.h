/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/07/hal-library-15-hd44780-for-stm32fxxx/
 * @version v1.0
 * @ide     Keil uVision
 * @license MIT
 * @brief   HD44780 LCD driver library for STM32Fxxx
 *
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_HD44780_H
#define TM_HD44780_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_HD44780
 * @brief    HD44780 LCD driver library for STM32Fxxx - http://stm32f4-discovery.net/2015/07/hal-library-15-hd44780-for-stm32fxxx/
 * @{
 *
 *	It also supports all HD44780 compatible LCD drivers.
 *
 * \par Default pinout
 *
\verbatim
LCD   STM32Fxxx         DESCRIPTION

GND   GND               Ground
VCC   +5V               Power supply for LCD
V0    Potentiometer	    Contrast voltage. Connect to potentiometer
RS    PB2               Register select, can be overwritten in your project's defines.h file
RW    GND               Read/write
E     PB7               Enable pin, can be overwritten in your project's defines.h file
D0    -                 Data 0 - doesn't care
D1    -                 Data 1 - doesn't care
D2    -                 Data 2 - doesn't care
D3    -                 Data 3 - doesn't  care
D4    PC12              Data 4, can be overwritten in your project's defines.h file
D5    PC13              Data 5, can be overwritten in your project's defines.h file
D6    PB12              Data 6, can be overwritten in your project's defines.h file
D7    PB13              Data 7, can be overwritten in your project's defines.h file
A     +3V3              Back light positive power
K     GND               Ground for back light
*/
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "stdlib.h"
#include "string.h"


// Длина строки дисплея
#define LENGTH_OF_LINE_LCD    	16
// Количество строк дисплея
#define NUMBER_OF_LINES_LCD   	2
// Текст, который будет выводиться на дисплей во время обновления
#define UPDATE_TEXT				"Идёт обновление!"
// Текст, который будет выводиться на дисплей после обновления
#define UPDATE_END_TEXT			"Готово!"
#define LEN_UPDATE_END_TEXT     strlen(UPDATE_END_TEXT)
// Демо-текст на первой строчке дисплея
#define DEMO_TEXT_1             "Демо текст 1"
// Демо-текст на второй строчке дисплея
#define DEMO_TEXT_2             "Demo text 2"
// Длина демо-текста №1
#define LEN_DEMO_TEXT_1         strlen(DEMO_TEXT_1)
// Длина демо-текста №2
#define LEN_DEMO_TEXT_2         strlen(DEMO_TEXT_2)
// Длина текста, выводимого во время обновления
#define LEN_UPDATE_TEXT			strlen(UPDATE_TEXT)

/* Pin defENABLEions */
#define HD44780_RS_LOW              HAL_GPIO_WritePin(HD44780_RS_PORT, HD44780_RS_PIN, GPIO_PIN_RESET)
#define HD44780_RS_HIGH             HAL_GPIO_WritePin(HD44780_RS_PORT, HD44780_RS_PIN, GPIO_PIN_SET)
#define HD44780_E_LOW               HAL_GPIO_WritePin(HD44780_E_PORT, HD44780_E_PIN, GPIO_PIN_RESET)
#define HD44780_E_HIGH              HAL_GPIO_WritePin(HD44780_E_PORT, HD44780_E_PIN, GPIO_PIN_SET)

#define HD44780_E_BLINK             HD44780_E_HIGH; HAL_Delay(1); HD44780_E_LOW; HAL_Delay(1)
#define HD44780_Delay(x)            osDelay(x)

/* Commands*/
#define HD44780_CLEARDISPLAY        0x01
#define HD44780_RETURNHOME          0x02
#define HD44780_ENTRYMODESET        0x04
#define HD44780_DISPLAYCONTROL      0x08
#define HD44780_CURSORSHIFT         0x10
#define HD44780_FUNCTIONSET         0x20
#define HD44780_SETCGRAMADDR        0x40
#define HD44780_SETDDRAMADDR        0x80

/* Flags for display entry mode */
#define HD44780_ENTRYRIGHT          0x00
#define HD44780_ENTRYLEFT           0x02
#define HD44780_ENTRYSHIFTINCREMENT 0x01
#define HD44780_ENTRYSHIFTDECREMENT 0x00

/* Flags for display on/off control */
#define HD44780_DISPLAYON           0x04
#define HD44780_CURSORON            0x02
#define HD44780_BLINKON             0x01

/* Flags for display/cursor shift */
#define HD44780_DISPLAYMOVE         0x08
#define HD44780_CURSORMOVE          0x00
#define HD44780_MOVERIGHT           0x04
#define HD44780_MOVELEFT            0x00

/* Flags for function set */
#define HD44780_8BITMODE            0x10
#define HD44780_4BITMODE            0x00
#define HD44780_2LINE               0x08
#define HD44780_1LINE               0x00
#define HD44780_5x10DOTS            0x04
#define HD44780_5x8DOTS             0x00

/* 4-х канальный режим управления LCD */
/* Управляющие контакты могут быть перезаписаны */
/* RS - Register select pin */
#ifndef HD44780_RS_PIN
#define HD44780_RS_PORT				GPIOA
#define HD44780_RS_PIN				GPIO_PIN_10
#endif
/* E - Enable pin */
#ifndef HD44780_E_PIN
#define HD44780_E_PORT				GPIOA
#define HD44780_E_PIN				GPIO_PIN_11
#endif
/* Выводы данных */
/* D4 - Data 4 pin */
#ifndef HD44780_D4_PIN
#define HD44780_D4_PORT				GPIOA
#define HD44780_D4_PIN				GPIO_PIN_12
#endif
/* D5 - Data 5 pin */
#ifndef HD44780_D5_PIN
#define HD44780_D5_PORT				GPIOA
#define HD44780_D5_PIN				GPIO_PIN_15
#endif
/* D6 - Data 6 pin */
#ifndef HD44780_D6_PIN
#define HD44780_D6_PORT				GPIOB
#define HD44780_D6_PIN				GPIO_PIN_3
#endif
/* D7 - Data 7 pin */
#ifndef HD44780_D7_PIN
#define HD44780_D7_PORT				GPIOB
#define HD44780_D7_PIN				GPIO_PIN_4
#endif

/**
 * @brief  Sets pin(s) low
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin low
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them low
 * @retval None
 */
#define TM_GPIO_SetPinLow(GPIOx, GPIO_Pin)			((GPIOx)->BSRR = (uint32_t)(((uint32_t)GPIO_Pin) << 16))

/**
 * @brief  Sets pin(s) high
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin high
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them high
 * @retval None
 */
#define TM_GPIO_SetPinHigh(GPIOx, GPIO_Pin)			((GPIOx)->BSRR = (uint32_t)(GPIO_Pin))

/**
 * @brief  Sets pin(s) value
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin value
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them value
 * @param  val: If parameter is 0 then pin will be low, otherwise high
 * @retval None
 */
#define TM_GPIO_SetPinValue(GPIOx, GPIO_Pin, val)	((val) ? TM_GPIO_SetPinHigh(GPIOx, GPIO_Pin) : TM_GPIO_SetPinLow(GPIOx, GPIO_Pin))

/**
 * @brief  Toggles pin(s)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to toggle pin value
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to toggle them all at a time
 * @retval None
 */
#define TM_GPIO_TogglePinValue(GPIOx, GPIO_Pin)		((GPIOx)->ODR ^= (GPIO_Pin))

/**
 * @brief  Sets value to entire GPIO PORT
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set value
 * @param  value: Value for GPIO OUTPUT data
 * @retval None
 */
#define TM_GPIO_SetPortValue(GPIOx, value)			((GPIOx)->ODR = (value))

/**
 * @brief  Gets input data bit
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read input bit value
 * @param  GPIO_Pin: GPIO pin where you want to read value
 * @retval 1 in case pin is high, or 0 if low
 */
#define TM_GPIO_GetInputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->IDR & (GPIO_Pin)) == 0 ? 0 : 1)

/**
 * @brief  Gets output data bit
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read output bit value
 * @param  GPIO_Pin: GPIO pin where you want to read value
 * @retval 1 in case pin is high, or 0 if low
 */
#define TM_GPIO_GetOutputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->ODR & (GPIO_Pin)) == 0 ? 0 : 1)

/**
 * @brief  Gets input value from entire GPIO PORT
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read input data value
 * @retval Entire PORT INPUT register
 */
#define TM_GPIO_GetPortInputValue(GPIOx)			((GPIOx)->IDR)

/**
 * @brief  Gets output value from entire GPIO PORT
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read output data value
 * @retval Entire PORT OUTPUT register
 */
#define TM_GPIO_GetPortOutputValue(GPIOx)			((GPIOx)->ODR)

/**
 * @defgroup TM_HD44780_Macros
 * @brief    Library defines
 * @{
 */

/**
 * @}
 */

/**
 * @defgroup TM_HD44780_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Initializes HD44780 LCD
 * @brief  cols: Width of lcd
 * @param  rows: Height of lcd
 * @retval None
 */
void TM_HD44780_Init(uint8_t cols, uint8_t rows);

/**
 * @brief  Turn display on
 * @param  None
 * @retval None
 */
void TM_HD44780_DisplayOn(void);

/**
 * @brief  Turn display off
 * @param  None
 * @retval None
 */
void TM_HD44780_DisplayOff(void);

/**
 * @brief  Clears entire LCD
 * @param  None
 * @retval None
 */
void TM_HD44780_Clear(void);

/**
 * @brief  Puts string on lcd
 * @param  x: X location where string will start
 * @param  y; Y location where string will start
 * @param  *str: pointer to string to display
 * @retval None
 */
void TM_HD44780_Puts(uint8_t x, uint8_t y, uint8_t * str, uint8_t len);

/**
 * @brief  Enables cursor blink
 * @param  None
 * @retval None
 */
void TM_HD44780_BlinkOn(void);

/**
 * @brief  Disables cursor blink
 * @param  None
 * @retval None
 */
void TM_HD44780_BlinkOff(void);

/**
 * @brief  Shows cursor
 * @param  None
 * @retval None
 */
void TM_HD44780_CursorOn(void);

/**
 * @brief  Hides cursor
 * @param  None
 * @retval None
 */
void TM_HD44780_CursorOff(void);

/**
 * @brief  Scrolls display to the left
 * @param  None
 * @retval None
 */
void TM_HD44780_ScrollLeft(void);

/**
 * @brief  Scrolls display to the right
 * @param  None
 * @retval None
 */
void TM_HD44780_ScrollRight(void);

/**
 * @brief  Creates custom character
 * @param  location: Location where to save character on LCD. LCD supports up to 8 custom characters, so locations are 0 - 7
 * @param *data: Pointer to 8-bytes of data for one character
 * @retval None
 */
void TM_HD44780_CreateChar(uint8_t location, uint8_t* data);

/**
 * @brief  Puts custom created character on LCD
 * @param  x: X location where character will be shown
 * @param  y: Y location where character will be shown
 * @param  location: Location on LCD where character is stored, 0 - 7
 * @retval None
 */
void TM_HD44780_PutCustom(uint8_t x, uint8_t y, uint8_t location);

void TM_HD44780_Puts_test(uint8_t x, uint8_t y, uint8_t *str, uint8_t len);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
