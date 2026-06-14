/*
 * imu_task.c
 *
 *  Created on: 23. svi 2026.
 *      Author: ANTONIA
 */

#include "imu_task.h"

#include <bmi270_STM.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"


osThreadId_t imuTaskHandle;

const osThreadAttr_t imuTaskAttributes = {
    .name = "imuTask",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4
};

void imuTask(void *argument)
{
	bmi270_STM_init();
    vTaskSuspend(NULL);
}
