/*
 * wifi_task.c
 *
 *  Created on: 23. svi 2026.
 *      Author: ANTONIA
 */

#include "wifi_task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

osThreadId_t wifiTaskHandle;
const osThreadAttr_t wifiTaskAttributes = {
    .name = "wifiTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};

void wifiTask(void *argument)
{
    vTaskSuspend(NULL);
}
