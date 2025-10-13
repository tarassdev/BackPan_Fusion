/*
 * task_imu.h
 *
 *  Created on: Oct 13, 2025
 *      Author: Serhii Filimoshyn
 */

#ifndef TASKS_TASK_IMU_H_
#define TASKS_TASK_IMU_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "drivers/flexcan0_drv.h"

void TaskIMU_Init(void);
void TaskIMU(void *pvParameters);



#endif /* TASKS_TASK_IMU_H_ */
