/*
 * sdcard_task.h
 *
 *  Created on: 23. svi 2026.
 *      Author: ANTONIA
 */

#ifndef INC_SDCARD_TASK_H_
#define INC_SDCARD_TASK_H_

#include "cmsis_os.h"

extern osThreadId_t sdcardTaskHandle;
extern const osThreadAttr_t sdcardTaskAttributes;

void sdcardTask(void *argument);

#endif /* INC_SDCARD_TASK_H_ */
