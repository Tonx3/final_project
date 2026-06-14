/*
 * sdcard_task.c
 *
 *  Created on: 23. svi 2026.
 *      Author: ANTONIA
 */

#include "sdcard_task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

osThreadId_t sdcardTaskHandle;
const osThreadAttr_t sdcardTaskAttributes = {
    .name = "sdcardTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};

void sdcardTask(void *argument)
{
    vTaskSuspend(NULL);
}
