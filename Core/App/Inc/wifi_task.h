/*
 * wifi_task.h
 *
 *  Created on: 23. svi 2026.
 *      Author: ANTONIA
 */

#ifndef INC_WIFI_TASK_H_
#define INC_WIFI_TASK_H_

#include "cmsis_os.h"

extern osThreadId_t wifiTaskHandle;
extern const osThreadAttr_t wifiTaskAttributes;

void wifiTask(void *argument);

#endif /* INC_WIFI_TASK_H_ */
