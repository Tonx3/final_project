/*
 * imu_task.h
 *
 *  Created on: 23. svi 2026.
 *      Author: ANTONIA
 */

#ifndef INC_IMU_TASK_H_
#define INC_IMU_TASK_H_

#include "cmsis_os.h"

extern osThreadId_t imuTaskHandle;
extern const osThreadAttr_t imuTaskAttributes;


void imuTask(void *argument);

#endif /* INC_IMU_TASK_H_ */
