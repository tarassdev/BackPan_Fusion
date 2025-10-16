/*
 * task_canopen.h
 *
 *  Created on: Oct 7, 2025
 *      Author: Serhii Filimoshyn
 */

#ifndef TASKS_TASK_CANOPEN_H_
#define TASKS_TASK_CANOPEN_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "drivers/flexcan1_drv.h"
#include "shared/shared_data.h"

void TaskCANopen_Init(void);
void TaskCANopen(void *pvParameters);

#endif /* TASKS_TASK_CANOPEN_H_ */
