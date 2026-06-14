/*
 * ads_task.h
 *
 *  Created on: 23. svi 2026.
 *      Author: ANTONIA
 */

#ifndef INC_ADS_TASK_H_
#define INC_ADS_TASK_H_

#include "cmsis_os.h"

extern osThreadId_t adsTaskHandle;
extern const osThreadAttr_t adsTaskAttributes;

void adsTask(void *argument);

#endif /* INC_ADS_TASK_H_ */
