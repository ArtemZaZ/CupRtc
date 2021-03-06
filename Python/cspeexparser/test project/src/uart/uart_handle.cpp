/***************************************************************************************************

 В этом файле находится реализация класса uart::Handle, который инкапсулирует самый низкий уровень
 работы с uart.

 Для доступа к отдельным uart'ам используйте глобальные объекты uart::uart1, uart::uart2 и т.д.

***************************************************************************************************/

#include "uart_handle.h"
#include <algorithm>


// это глобальные хендлы для нужных уартов
namespace uart
{
#ifdef UART_N1_ENABLE
    Handle uart1;
#endif

#ifdef UART_N2_ENABLE
    Handle uart2;
#endif

#ifdef UART_N3_ENABLE
    Handle uart3;
#endif

#ifdef UART_N4_ENABLE
    Handle uart4;
#endif

#ifdef UART_N5_ENABLE
    Handle uart5;
#endif
}


namespace uart
{

    // вспомогательная структура для настройки UART
    struct Handle :: UartSettings
    {
        // sort of fail-safe
        UartSettings() : uartInitStruct(),
                         portRx(0),
                         portTx(0),
                         pin_RX(0xFFFF),
                         pin_TX(0xFFFF),
                         usart(0),
                         uartRccPeriph(0xFFFFFFFF),
                         uart_rcc_Cmd(0),
                         isRemap(false),
                         remapConfig(0xFFFFFFFF),
                         uartIrqNum(SysTick_IRQn),
                         irqPrio(IRQ_PRIORITY_LEVEL_LOWEST)
        {;}


        USART_InitTypeDef uartInitStruct;

        GPIO_TypeDef * portRx;
        GPIO_TypeDef * portTx;

        uint16_t pin_RX;
        uint16_t pin_TX;

        USART_TypeDef * usart;

        uint32_t uartRccPeriph;

        void (* uart_rcc_Cmd)(uint32_t Rcc_Periph, FunctionalState NewState);

        bool isRemap;
        uint32_t remapConfig;

        IRQn uartIrqNum;

        uint8_t irqPrio;
    };


    // это счетчик уже созданных хендлов
    // к сожалению, статические переменные класса нельзя инициализировать иначе
    uint8_t Handle::handlesCounter = 0;



    /***************************************************************************************************
    ****************************************************************************************************

                                Публичные методы

    ****************************************************************************************************
    ***************************************************************************************************/




    /**************************************************************************************************
    Описание:  Конструктор по-умолчанию для класса uart::Handle
    Аргументы: -
    Возврат:   -
    Замечания: Использует счетчик и не позволяет создать экземпляров больше, чем разрешено с помощью
               директив вида UART_N1_ENABLE. Эти директивы должны быть в файле uart_settings.h
    **************************************************************************************************/
    Handle :: Handle() :
            m_isInited(false),
            m_uartRegs(0),
            m_curSettings(),
            m_error(error_none),
            m_pTxBuf(0),
            m_txCount(0),
            m_txMsgSize(0),
            m_isTransmitComplete(true),
            m_txBuffer(),
            m_rs485Port(0),
            m_rs485Pin(0),
            m_isRxOverrun(false),
        #ifdef UART_USE_FREERTOS
            m_rxQueue(0),
            m_ownerMutex(0),
            m_transmitCompleteSem(0) // здесь специально нет запятой
        #else
            rxBuffer(),
            m_isLocked(false)
        #endif
    {
        handlesCounter++;
        
        // этот енум нужен только ради значения MAX - максимального количества uart'ов
        // он не стронг, потому что внутри ифдефы
        enum UartNumbers {
                            #ifdef UART_N1_ENABLE
                                        UART_NUMBER_N_1,
                            #endif
                            #ifdef UART_N2_ENABLE
                                        UART_NUMBER_N_2,
                            #endif
                            #ifdef UART_N3_ENABLE
                                        UART_NUMBER_N_3,
                            #endif
                            #ifdef UART_N4_ENABLE
                                        UART_NUMBER_N_4,
                            #endif
                            #ifdef UART_N5_ENABLE
                                        UART_NUMBER_N_5,
                            #endif
                                        UART_NUMBER_MAX
        };

        // нельзя создавать уартов больше, чем разрешено с помощью констант UART_Nx_ENABLE
        UMBA_ASSERT( handlesCounter <= UART_NUMBER_MAX );
    }

    /**************************************************************************************************
    Описание:  Метод простой инициализации UART
    Аргументы: pins - пины для uart'a
               baudrate - бодрейт
               irqPrio - приоритет прерывания (по-умолчанию равен IRQ_PRIORITY_LEVEL_NORMAL)
    Возврат:   -
    Замечания: Подвисает, если номер uart'а и пины не совпадают
               Подвисает при попытке повторной инициализации
               На время инициализации диспетчер останавливается!
    **************************************************************************************************/
    void Handle :: init( Pins pins, uint32_t baudrate, GPIO_TypeDef * rs485Port, uint16_t rs485Pin, uint8_t irqPrio )
    {
        USART_InitTypeDef initStruct;

        // задаем настройки по-умолчанию
        initStruct.USART_BaudRate = baudrate;
        initStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        initStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        initStruct.USART_Parity = USART_Parity_No;
        initStruct.USART_StopBits = USART_StopBits_1;
        initStruct.USART_WordLength = USART_WordLength_8b;

        initExtended(pins, initStruct, rs485Port, rs485Pin, irqPrio);
    }

    /**************************************************************************************************
    Описание:  Метод расширенной инициализации UART
    Аргументы: pins - пины для uart'a
               initStruct - структура с параметрами для настройки
               irqPrio - приоритет прерывания (по-умолчанию равен IRQ_PRIORITY_LEVEL_NORMAL)
    Возврат:   -
    Замечания: assert, если номер uart'а и пины не совпадают
               assert при попытке повторной инициализации
               На время инициализации диспетчер останавливается!
    **************************************************************************************************/
    void Handle :: initExtended( Pins pins, USART_InitTypeDef & initStruct, GPIO_TypeDef * rs485Port, uint16_t rs485Pin, uint8_t irqPrio )
    {
        #ifdef USE_FREERTOS
                ENTER_UNSCHEDULED_SECTION();
        #endif

        m_rs485Pin = rs485Pin;
        m_rs485Port = rs485Port;

        // если это необходимо - настроим ногу для 485
        if(rs485Port != 0)
        {        
            RCC_APB2PeriphClockCmd( gpioToRcc(rs485Port), ENABLE);
            
            GPIO_InitTypeDef gpioInit;
            GPIO_StructInit( &gpioInit );

            gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
            gpioInit.GPIO_Pin = rs485Pin;
            gpioInit.GPIO_Speed = GPIO_Speed_10MHz;

            GPIO_Init( rs485Port, &gpioInit);
        }

        initPrivate(pins, initStruct, irqPrio);

        #ifdef USE_FREERTOS
                EXIT_UNSCHEDULED_SECTION();
        #endif
    }




    /**************************************************************************************************
    Описание:  Метод для смены бодрейта
    Аргументы: baudrate - новый бодрейт
    Возврат:   -
    Замечания: assert, если uart не иницализирован
               assert, если в момент вызова идет передача
               Обнуляет все очереди, счетчики и сбрасывает ошибки
    **************************************************************************************************/
    void Handle :: changeBaudrate( uint32_t baudrate )
    {
        UMBA_ASSERT(m_isInited);

        UMBA_ASSERT( isTransmitComplete() );

        // все обнуляем
        teardown();

        ENTER_CRITICAL_SECTION();

            USART_Cmd(m_uartRegs, DISABLE);

            m_curSettings.USART_BaudRate = baudrate;

            USART_Init(m_uartRegs, &m_curSettings);

            // включаем uart обратно
            USART_Cmd(m_uartRegs, ENABLE);

        EXIT_CRITICAL_SECTION();
    }

    /**************************************************************************************************
    Описание:  Метод сброса uart. Сбрасывает ошибки в периферии и все очереди/счетчики в хендле
    Аргументы: -
    Возврат:   -
    Замечания: assert, если uart не иницализирован
               assert, если в момент вызова идет передача
               Содержит критическую секцию
    **************************************************************************************************/
    void Handle :: reset(void)
    {
        UMBA_ASSERT(m_isInited);

        UMBA_ASSERT( isTransmitComplete() );

        ENTER_CRITICAL_SECTION();

            // выключаем uart
            USART_Cmd(m_uartRegs, DISABLE);

            teardown();

            // все ошибки должны сброситься от чтения регистра статуса
            // и чтения регистра данных
            USART_GetFlagStatus(m_uartRegs, USART_FLAG_ORE);
            USART_ReceiveData(m_uartRegs);

            // включаем uart обратно
            USART_Cmd(m_uartRegs, ENABLE);

        EXIT_CRITICAL_SECTION();
    }

    /**************************************************************************************************
    Описание:  Метод включения приемника
    Аргументы: -
    Возврат:   -
    Замечания: assert, если uart не иницализирован
    **************************************************************************************************/
    void Handle :: enableReceiver(void)
    {
        UMBA_ASSERT(m_isInited);

        // включаем приемник
        m_uartRegs->CR1 |= USART_CR1_RE;

        // разрешаем прерывание по приему
        m_uartRegs->CR1 |= USART_CR1_RXNEIE;

        m_curSettings.USART_Mode |= USART_Mode_Rx;
    }

    /**************************************************************************************************
    Описание:  Метод выключения приемника
    Аргументы: -
    Возврат:   -
    Замечания: assert, если uart не иницализирован
    **************************************************************************************************/
    void Handle :: disableReceiver(void)
    {
        UMBA_ASSERT(m_isInited);

        // если выключение произойдет во время приема байта
        // просто байт потеряется

        // запрещаем прерывание по приему
        m_uartRegs->CR1 &= ~USART_CR1_RXNEIE;

        // выключаем приемник
        m_uartRegs->CR1 &= ~USART_CR1_RE;

        m_curSettings.USART_Mode &= ~USART_Mode_Rx;
    }

    #ifdef UART_USE_FREERTOS


        QueueHandle_t Handle :: getQueue(void)
        {
            UMBA_ASSERT(m_isInited);

            return m_rxQueue;
        }

        SemaphoreHandle_t Handle :: getOwnerMutex(void)
        {
            UMBA_ASSERT(m_isInited);

            return m_ownerMutex;
        }

        SemaphoreHandle_t Handle :: getTransmitCompleteSem(void)
        {
            UMBA_ASSERT(m_isInited);

            return m_transmitCompleteSem;
        }

    #else


        bool Handle :: isNewByte(void)
        {
            UMBA_ASSERT(m_isInited);

            return ! rxBuffer.isEmpty();
        }

        uint8_t Handle :: getByte(void)
        {
            UMBA_ASSERT(m_isInited);

            UMBA_ASSERT( isNewByte() );

            return rxBuffer.readTail();
        }

        // "мьютекс" на владение
        bool Handle :: isLocked(void)
        {
            UMBA_ASSERT(m_isInited);

            return m_isLocked;
        }

        void Handle :: lock(void)
        {
            UMBA_ASSERT(m_isInited);

            m_isLocked = true;
        }

        void Handle :: unLock(void)
        {
            UMBA_ASSERT(m_isInited);

            m_isLocked = false;
        }

    #endif

    /**************************************************************************************************
    Описание:  Метод для отправки статического массива байт по uart

    Аргументы: statirArr - указатель на массив
               size - размер массива в байтах

    Возврат:   true - если массив успешно отправлен
               false - если в данный момент уже что-то отправляется

    Замечания: assert, если uart не иницализирован
               Статические массивы отправляются без копирования, копируется только указатель.
               Если массив на самом деле не живет достаточно долго - отправлятся будет черт знает что.
    **************************************************************************************************/
    bool Handle :: sendStaticArray( const volatile uint8_t * staticArr, uint32_t size )
    {
        UMBA_ASSERT(m_isInited);
        
        UMBA_ASSERT(size != 0);

        // предыдущее сообщение еще отправляется
        if( ! isTransmitComplete() )
        {
            return false;
        }

        #ifdef UART_USE_FREERTOS

            // нужно на всякий случай неблокирующе взять семафор отправки, иначе он будет врать
            osSemaphoreTake( m_transmitCompleteSem, 0);

        #endif

        // пусть этот барьер будет здесь на всякий случай
        DATA_SYNCHRONIZATION_BARRIER();

        m_isTransmitComplete = false;

        m_txCount = 0;
        m_txMsgSize = size;

        // просто копируем указатель - ведь массив _должен_ быть статическим
        m_pTxBuf = staticArr;

        // 1 байт отправляется прямо здесь
        m_txCount++;

        // разрешаем передачу - если нужно
        enableTransmit485();

        // и счетчик должен проинкрементиться ДО отправки байта
        DATA_SYNCHRONIZATION_BARRIER();

        m_uartRegs->DR = staticArr[0];

        // остальные - в прерывании
        // начали передачу
        return true;
    }

    /**************************************************************************************************
    Описание:  Метод для отправки локального массива байт по uart

    Аргументы: statirArr - указатель на массив
               size - размер массива в байтах

    Возврат:   true - если массив успешно отправлен
               false - если в данный момент уже что-то отправляется

    Замечания: assert, если uart не иницализирован
               assert, если массив не влезает в отправной буфер. Локальный массив сначала копируется,
               поэтому его размер не должен превышать tx_buffer_size
    **************************************************************************************************/
    bool Handle :: sendLocalArray( const uint8_t * localArr, uint32_t size )
    {
        UMBA_ASSERT(m_isInited);
        
        UMBA_ASSERT(size != 0);

        // массив должен влезать в наш буфер
        UMBA_ASSERT( size <= tx_buffer_size );

        if(! isTransmitComplete())
        {
            return false;
        }

        DATA_SYNCHRONIZATION_BARRIER();

        // теоретически, прерывание после этой строки происходить не должно

        // копируем весь массив
        std::copy( localArr, localArr + size, m_txBuffer );

        // и посылаем его как статический
        return sendStaticArray(m_txBuffer, size);
    }

    bool Handle :: isTransmitComplete(void)
    {
        UMBA_ASSERT(m_isInited);

        return m_isTransmitComplete;
    }

    bool Handle :: isReceiverOverrun(void)
    {
        UMBA_ASSERT(m_isInited);

        return m_isRxOverrun;
    }

    /**************************************************************************************************
    Описание:  Метод для получения ошибки, произошедшей в uart.

    Аргументы: -
    Возврат:   uart::error_none - если ошибок нет
               Или комбинация из остальных ошибок. Коды ошибок перечислены в файле uart_handle.h
    Замечания: assert, если uart не иницализирован
    **************************************************************************************************/
    uint32_t Handle :: getError(void)
    {
        UMBA_ASSERT(m_isInited);

        return m_error;
    }

    /**************************************************************************************************
    Описание:  Метод-обработчик прерывания

    Аргументы: -
    Возврат:   -
    Замечания: assert, если uart не иницализирован
    **************************************************************************************************/
    void Handle :: irqHandler(void)
    {
        UMBA_ASSERT(m_isInited);


        uint16_t status =  m_uartRegs->SR;

        // не произошла ли ошибка?
        // если произошла - то сбросить ее можно только с помощью функции reset
        // ошибки кодируют только биты 3..0
        // правда оверран может быть сброшен в конце обработчика
        m_error = status & 0xF;

        // прием
        if( status & USART_SR_RXNE )
        {
            uint8_t byte = m_uartRegs->DR;

            // если в приемнике нет места - байт теряется и выставляется флаг overrun
            #ifdef UART_USE_FREERTOS

                BaseType_t result = osQueueSendToBackFromISR(m_rxQueue, &byte, NULL);

                if(result == errQUEUE_FULL)
                {
                    m_isRxOverrun = true;
                }

            #else

                if( rxBuffer.isFull() )
                {
                    m_isRxOverrun = true;
                }
                else
                {
                    rxBuffer.writeHead(byte);
                }

            #endif
        }

        // передача
        if( status & USART_SR_TC )
        {
            USART_ClearITPendingBit(m_uartRegs, USART_IT_TC);

            // все отправлено
            if( m_txCount == m_txMsgSize)
            {
                m_pTxBuf = 0;
                m_isTransmitComplete = true;

                disableTransmit485();

                #ifdef UART_USE_FREERTOS

                    osSemaphoreGiveFromISR(m_transmitCompleteSem, NULL);

                #endif

                return;
            }

            // еще есть, что отправить
            UMBA_ASSERT(m_pTxBuf);

            m_uartRegs->DR = m_pTxBuf[ m_txCount ];
            m_txCount++;
        }
        
        // оверран мог произойти между чтением регистра статуса и чтением байта из DR
        // в таком случае прерывание зациклится. Этого не должно происходить.
        if( m_uartRegs->SR & USART_SR_ORE)
        {
            // не совсем понятно, валидный ли этот байт или нет
            // для простоты будем его выкидывать
            volatile uint8_t byte = m_uartRegs->DR;
            (void)byte;
        }
    }

    /***************************************************************************************************
    ****************************************************************************************************

                                Приватные методы

    ****************************************************************************************************
    ***************************************************************************************************/

    /**************************************************************************************************
    Описание:  Приватный метод для сброса очередей, счетчиков, ошибок и т.д.
    Аргументы: -
    Возврат:   -
    Замечания: assert, если uart не иницализирован
               assert, если в момент вызова идет передача
               Содержит критическую секцию
    **************************************************************************************************/
    void Handle :: teardown(void)
    {
        UMBA_ASSERT(m_isInited);

        UMBA_ASSERT( isTransmitComplete() );

        // обнуляем все приемные и передающие очереди
        ENTER_CRITICAL_SECTION();

            m_error = error_none;

            // больше отправлять ничего не надо
            m_txCount = 0;
            m_txMsgSize = 0;
            m_pTxBuf = 0;
            m_isTransmitComplete = true;

            // TODO сбрасывать мьютексы тоже нужно
            #ifdef UART_USE_FREERTOS

                // сбрасывать ее вроде бы безопасно даже если кто-то ждет ее
                osQueueReset(m_rxQueue);

            #else

                // обнуляем приемник
                rxBuffer.reset();

            #endif

        EXIT_CRITICAL_SECTION();
    }

    /**************************************************************************************************
    Описание:  Вспомогательная функция для настройки uart'a
    Аргументы: Ссылка на структуру с настройками
    Возврат:   -
    Замечания: -
    **************************************************************************************************/
    void Handle :: uartInitHelper( Handle :: UartSettings & settings )
    {
        // настраиваем ноги

        GPIO_InitTypeDef gpioInit;
        GPIO_StructInit( &gpioInit );

        // тактирование
        RCC_APB2PeriphClockCmd( gpioToRcc(settings.portRx), ENABLE);
        RCC_APB2PeriphClockCmd( gpioToRcc(settings.portTx), ENABLE);

        if(settings.isRemap)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
            GPIO_PinRemapConfig(settings.remapConfig, ENABLE);
        }


        // инициализация RX

        gpioInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        gpioInit.GPIO_Pin = settings.pin_RX;
        gpioInit.GPIO_Speed = GPIO_Speed_50MHz;

        GPIO_Init(settings.portRx, &gpioInit);

        // инициализаия ТХ

        gpioInit.GPIO_Mode = GPIO_Mode_AF_PP;
        gpioInit.GPIO_Pin = settings.pin_TX;
        gpioInit.GPIO_Speed = GPIO_Speed_50MHz;

        GPIO_Init(settings.portTx, &gpioInit);


        // настраиваем уарт

        // подаем тактирование на уарт
        settings.uart_rcc_Cmd( settings.uartRccPeriph, ENABLE);

        // инициализируем настройки уарта
        USART_Init( settings.usart, & settings.uartInitStruct);

        // прерывания - принят байт и отправлен байт
        USART_ITConfig(settings.usart, USART_IT_TC, ENABLE);
        USART_ITConfig(settings.usart, USART_IT_RXNE, ENABLE);

        USART_Cmd(settings.usart, ENABLE);
        
        // прерывание может сейчас сработать и этот флаг проверить
        // поэтому выставляем его чуть-чуть заранее
        m_isInited = true;
        
        NVIC_SetPriority(settings.uartIrqNum, settings.irqPrio);

        NVIC_EnableIRQ(settings.uartIrqNum);
    }

    /**************************************************************************************************
    Описание:  Приватный метод, в котором, собственно, происходит инициализация. Он нужен только
               для удобства оборачивания в UNSCHEDULED_SECTION.
    Аргументы:
    Возврат:
    Замечания:
    **************************************************************************************************/
    void Handle :: initPrivate(Pins pins, USART_InitTypeDef & initStruct, uint8_t irqPrio)
    {
        // второй раз инит делать нельзя
        UMBA_ASSERT( ! m_isInited);

        #ifdef USE_FREERTOS

            UMBA_ASSERT(irqPrio >= IRQ_PRIORITY_LEVEL_SYSCALL);

        #endif

        m_curSettings = initStruct;

        #ifdef UART_USE_FREERTOS
            // нужно создать очередь из байтов
            m_rxQueue = osQueueCreate( rx_buffer_size, 1 );

            UMBA_ASSERT(m_rxQueue);

            // мьютекс на владение
            m_ownerMutex = osSemaphoreCreateMutex();

            UMBA_ASSERT(m_ownerMutex);

            // семафор на завершение передачи
            // семафор создается обнуленным
            m_transmitCompleteSem = osSemaphoreCreateBinary();

            UMBA_ASSERT(m_transmitCompleteSem);
        #endif

        UartSettings settings;

        settings.uartInitStruct = initStruct;
        settings.irqPrio = irqPrio;

        switch(pins)
        {

        #ifdef UART_N1_ENABLE

            case Pins::UART1_PA9_PA10:

                UMBA_ASSERT(this == &uart1);

                m_uartRegs = USART1;

                settings.portRx = GPIOA;
                settings.portTx = GPIOA;
                settings.isRemap = false;
                settings.remapConfig = 0;

                settings.pin_RX = GPIO_Pin_10;
                settings.pin_TX = GPIO_Pin_9;

                settings.uartRccPeriph = RCC_APB2Periph_USART1;
                settings.usart = USART1;
                settings.uart_rcc_Cmd = RCC_APB2PeriphClockCmd;
                settings.uartIrqNum = USART1_IRQn;

                break;

            case Pins::UART1_PB6_PB7:

                UMBA_ASSERT(this == &uart1);

                m_uartRegs = USART1;

                settings.portRx = GPIOB;
                settings.portTx = GPIOB;
                settings.isRemap = true;
                settings.remapConfig  = GPIO_Remap_USART1;

                settings.pin_RX = GPIO_Pin_7;
                settings.pin_TX = GPIO_Pin_6;

                settings.uartRccPeriph = RCC_APB2Periph_USART1;
                settings.usart = USART1;
                settings.uart_rcc_Cmd = RCC_APB2PeriphClockCmd;
                settings.uartIrqNum = USART1_IRQn;

                break;

        #endif

        #ifdef UART_N2_ENABLE

            case Pins::UART2_PA2_PA3:

                UMBA_ASSERT(this == &uart2);

                m_uartRegs = USART2;

                settings.portRx = GPIOA;
                settings.portTx = GPIOA;
                settings.isRemap = false;
                settings.remapConfig  = 0;

                settings.pin_RX = GPIO_Pin_3;
                settings.pin_TX = GPIO_Pin_2;

                settings.uartRccPeriph = RCC_APB1Periph_USART2;
                settings.usart = USART2;
                settings.uart_rcc_Cmd = RCC_APB1PeriphClockCmd;
                settings.uartIrqNum = USART2_IRQn;

                break;

            case Pins::UART2_PD5_PD6:

                UMBA_ASSERT(this == &uart2);

                m_uartRegs = USART2;

                settings.portRx = GPIOD;
                settings.portTx = GPIOD;
                settings.isRemap = true;
                settings.remapConfig  = GPIO_Remap_USART2;

                settings.pin_RX = GPIO_Pin_6;
                settings.pin_TX = GPIO_Pin_5;

                settings.uartRccPeriph = RCC_APB1Periph_USART2;
                settings.usart = USART2;
                settings.uart_rcc_Cmd = RCC_APB1PeriphClockCmd;
                settings.uartIrqNum = USART2_IRQn;

                break;

        #endif

        #ifdef UART_N3_ENABLE

            case Pins::UART3_PB10_PB11:

                UMBA_ASSERT(this == &uart3);

                m_uartRegs = USART3;

                settings.portRx = GPIOB;
                settings.portTx = GPIOB;
                settings.isRemap = false;
                settings.remapConfig  = 0;

                settings.pin_RX = GPIO_Pin_11;
                settings.pin_TX = GPIO_Pin_10;

                settings.uartRccPeriph = RCC_APB1Periph_USART3;
                settings.usart = USART3;
                settings.uart_rcc_Cmd = RCC_APB1PeriphClockCmd;
                settings.uartIrqNum = USART3_IRQn;

                break;

            case Pins::UART3_PC10_PC11:

                UMBA_ASSERT(this == &uart3);

                m_uartRegs = USART3;

                settings.portRx = GPIOC;
                settings.portTx = GPIOC;
                settings.isRemap = true;
                settings.remapConfig  = GPIO_PartialRemap_USART3;

                settings.pin_RX = GPIO_Pin_11;
                settings.pin_TX = GPIO_Pin_10;

                settings.uartRccPeriph = RCC_APB1Periph_USART3;
                settings.usart = USART3;
                settings.uart_rcc_Cmd = RCC_APB1PeriphClockCmd;
                settings.uartIrqNum = USART3_IRQn;

                break;

            case Pins::UART3_PD8_PD9:

                UMBA_ASSERT(this == &uart3);

                m_uartRegs = USART3;

                settings.portRx = GPIOD;
                settings.portTx = GPIOD;
                settings.isRemap = true;
                settings.remapConfig  = GPIO_FullRemap_USART3;

                settings.pin_RX = GPIO_Pin_9;
                settings.pin_TX = GPIO_Pin_8;

                settings.uartRccPeriph = RCC_APB1Periph_USART3;
                settings.usart = USART3;
                settings.uart_rcc_Cmd = RCC_APB1PeriphClockCmd;
                settings.uartIrqNum = USART3_IRQn;

                break;

        #endif

        #ifdef UART_N4_ENABLE

            case Pins::UART4_PC10_PC11:

                UMBA_ASSERT(this == &uart4);

                m_uartRegs = UART4;

                settings.portRx = GPIOC;
                settings.portTx = GPIOC;
                settings.isRemap = false;
                settings.remapConfig  = 0;

                settings.pin_RX = GPIO_Pin_11;
                settings.pin_TX = GPIO_Pin_10;

                settings.uartRccPeriph = RCC_APB1Periph_UART4;
                settings.usart = UART4;
                settings.uart_rcc_Cmd = RCC_APB1PeriphClockCmd;
                settings.uartIrqNum = UART4_IRQn;

                break;

        #endif

        #ifdef UART_N5_ENABLE

            case Pins::UART5_PC12_PD2:

                UMBA_ASSERT(this == &uart5);

                m_uartRegs = UART5;

                settings.portRx = GPIOD;
                settings.portTx = GPIOC;

                settings.isRemap = false;
                settings.remapConfig  = 0;

                settings.pin_RX = GPIO_Pin_2;
                settings.pin_TX = GPIO_Pin_12;

                settings.uartRccPeriph = RCC_APB1Periph_UART5;
                settings.usart = UART5;
                settings.uart_rcc_Cmd = RCC_APB1PeriphClockCmd;
                settings.uartIrqNum = UART5_IRQn;


                break;

        #endif

            default:

                // вы пытаетесь инициализировать неразрешенный uart
                UMBA_ASSERT_FAIL();
                break;

        } // switch(pins)

        uartInitHelper(settings);
        
    } // initPrivate


    void Handle :: enableTransmit485(void)
    {
        if(m_rs485Port != 0)
        {
            m_rs485Port->BSRR = m_rs485Pin;
        }
    }

    void Handle :: disableTransmit485(void)
    {
        if(m_rs485Port != 0)
        {
            m_rs485Port->BRR = m_rs485Pin;
        }
    }

} // namespace uart


/***************************************************************************************************
  Перехват обработчиков прерываний
***************************************************************************************************/

extern "C"
{

#ifdef UART_N1_ENABLE
    void USART1_IRQHandler(void)
    {
        uart::uart1.irqHandler();
    }
#endif

#ifdef UART_N2_ENABLE
    void USART2_IRQHandler(void)
    {
        uart::uart2.irqHandler();
    }
#endif

#ifdef UART_N3_ENABLE
    void USART3_IRQHandler(void)
    {
        uart::uart3.irqHandler();
    }
#endif

#ifdef UART_N4_ENABLE
    void UART4_IRQHandler(void)
    {
        uart::uart4.irqHandler();
    }
#endif

#ifdef UART_N5_ENABLE
    void UART5_IRQHandler(void)
    {
        uart::uart5.irqHandler();
    }
#endif




}
