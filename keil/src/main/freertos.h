#ifndef FREERTOS_H
#define FREERTOS_H

#include "include/FreeRTOS.h"
#include "include/task.h"
#include "include/queue.h"
#include "include/semphr.h"
#include "cmsis_os.h"
#include "string.h"
#include "Prj_config.h"

void freertos_init(void);

#endif
