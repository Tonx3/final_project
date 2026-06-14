/*
 * main_task.c
 *
 *  Created on: 12. tra 2026.
 *      Author: ANTONIA
 */
#include <stdio.h>

#include "main_task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

osThreadId_t mainTaskHandle;
const osThreadAttr_t mainTaskAttributes = {
  .name = "mainTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

void mainTask(void *argument)
{
	// TODO?
	vTaskSuspend(NULL);
}
