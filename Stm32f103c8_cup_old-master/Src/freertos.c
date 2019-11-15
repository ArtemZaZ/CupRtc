/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * Copyright (c) 2018 STMicroelectronics International N.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "defines.h"
#include "STM32_EEPROM_SPI.h"

#ifdef WS2812B_ENABLE
#include "ws2812b.h"
#endif

#ifdef LCD_ENABLE
#include "tm_stm32_hd44780.h"
#endif

#ifdef AUDIO_ENABLE
#include "audio.h"
#include "spx.h"

#endif

#ifdef LED_MATRIX_ENABLE
#include "max7219.h"
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

// Режим работы кубка. 0-нормальная работа, 1-демо режим, 2-режим обновления
extern uint8_t systemMode;
uint8_t playSound = 0;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId RGBws2812bHandle;
osThreadId AudioMessageHandle;
osThreadId ButtonHandle;
extern uint8_t eightbit[766][3];
extern TIM_HandleTypeDef htim4;

osSemaphoreId chipSelectSemHandle;

extern const uint16_t spx_frames2;
extern const uint8_t spx_voice2[2820];

/* USER CODE END Variables */
osThreadId LCDTaskHandle;
osMessageQId myQueue01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
#ifdef WS2812B_ENABLE
void StartRGBws2812bTask(void const * argument);
#endif

#ifdef AUDIO_ENABLE
void StartAudioMessageTask(void const * argument);
#endif

void StartButtonTask(void const * argument);
/* USER CODE END FunctionPrototypes */

void StartLCDTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	osSemaphoreDef(chipSelectSem);
	chipSelectSemHandle = osSemaphoreCreate(osSemaphore(chipSelectSem), 1);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of LCDTask */
  osThreadDef(LCDTask, StartLCDTask, osPriorityNormal, 0, 128);
  LCDTaskHandle = osThreadCreate(osThread(LCDTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  osThreadDef(Button, StartButtonTask, osPriorityNormal, 0, 256);
  ButtonHandle = osThreadCreate(osThread(Button), NULL);

#ifdef	WS2812B_ENABLE
	/* definition and creation of RGBws2812b */
	osThreadDef(RGBws2812b, StartRGBws2812bTask, osPriorityNormal, 0, 512);
	RGBws2812bHandle = osThreadCreate(osThread(RGBws2812b), NULL);
#endif

#ifdef	AUDIO_ENABLE
	/* definition and creation of AudioMessage */
	osThreadDef(AudioMessage, StartAudioMessageTask, osPriorityNormal, 0, 256);
	AudioMessageHandle = osThreadCreate(osThread(AudioMessage), NULL);
#endif

  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of myQueue01 */
/* what about the sizeof here??? cd native code */
  osMessageQDef(myQueue01, 16, uint16_t);
  myQueue01Handle = osMessageCreate(osMessageQ(myQueue01), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_StartLCDTask */
/**
 * @brief  Function implementing the LCDTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartLCDTask */
void StartLCDTask(void const * argument)
{

  /* USER CODE BEGIN StartLCDTask */
	/* Infinite loop */
	for (;;) {
#ifndef LCD_ENABLE
		vTaskSusped(NULL);
#endif
#ifdef LCD_ENABLE	// Вывод сообщения на дисплей

		TM_HD44780_Clear();
		TM_HD44780_Puts(0, 0, (uint8_t*) "Team N", strlen("Team N"));
		TM_HD44780_Puts(0, 1, (uint8_t*) "Member n1", strlen("Member n1"));
		osDelay(5000);
		TM_HD44780_Clear();
		TM_HD44780_Puts(0, 0, (uint8_t*) "Team N", strlen("Team N"));
		TM_HD44780_Puts(0, 1, (uint8_t*) "Member n2", strlen("Member n2"));
		osDelay(5000);

#endif
	}
  /* USER CODE END StartLCDTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* StartButtonTask function */
void StartButtonTask(void const * argument)		//задача для опрашивания кнопки
{
	// Создаем счётчик для фиксации времени нажатия кнопки
	uint8_t pressTime;
	uint8_t col = 64;

	for(;;)
	{
		if(AntiContactBounce(BUTTON_GPIO_PORT, BUTTON_GPIO_PIN))	//против дребезга контактов
		{
			pressTime = 1;
			// Крутим счётчик до тех пор пока зажата кнопка или
			// значение счётчика меньше 255
			while((BUTTON_GPIO_PORT->IDR & BUTTON_GPIO_PIN)&&(pressTime != 255))
			{
				osDelay(100);
				pressTime++;
			}
			// Если было одно нажатие, то выключаем светодиоды на 1с
			if((pressTime <= 30)&&(pressTime >= 1))
			{
				WS2812_send_group(0, 0, 0,
						0, 0, 0,
						0, 0, 0,
						0, 0, 0);

				osDelay(1000);
				WS2812_send_group(col, 0, 0,
								  0, col, 0,
								  col/2, 0, col/2,
								  0, 0, col);
			}
		}
		osDelay(10);
	}
	/* USER CODE END StartButtonTask */
}


/* USER CODE BEGIN Application */
#ifdef WS2812B_ENABLE
/* StartRGBws2812bTask function */
void StartRGBws2812bTask(void const * argument) {	//задача для управления светодиодами
	/* USER CODE BEGIN StartRGBws2812bTask */
	uint8_t col = 64; //величина яркости одного светодиода (макс = 255)
	WS2812_send_group(col, 0, 0,	//отправка цветов светодиодов по 4м группам
					  0, col, 0,
					  col/2, 0, col/2,
					  0, 0, col);
	/* Infinite loop */
	for (;;) {

	}
	/* USER CODE END StartRGBws2812bTask */
}
#endif

#ifdef AUDIO_ENABLE

void StartAudioMessageTask(void const * argument)	//задача для проигрывания аудио-сообщения
{
	//Для записи сообщения в eeprom
	//EEPROM_SPI_WriteBuffer((uint8_t*)&spx_voice2[0], (uint16_t)0x01, (uint16_t)(sizeof(spx_voice2)));
	for(;;)
	{
		// Включаем прерывания по таймеру
		HAL_TIM_Base_Start_IT(&htim4);
		// Начинаем проигрывать запись
		PlayMessageFromEEPROM();	//из eeprom
		//PlayMessage(spx_voice2, spx_frames2);	//из .h файла
		// Выключаем прерывания по таймеру
		HAL_TIM_Base_Stop_IT(&htim4);
		osDelay(5000);
	}
}
#endif

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
