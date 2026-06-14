/*
 * main_task.h
 *
 *  Created on: 12. tra 2026.
 *      Author: ANTONIA
 */

#ifndef APP_INC_MAIN_TASK_H_
#define APP_INC_MAIN_TASK_H_


#include "cmsis_os.h"

extern osThreadId_t mainTaskHandle;
extern const osThreadAttr_t mainTaskAttributes;


void mainTask(void *argument);


#endif /* APP_INC_MAIN_TASK_H_ */
