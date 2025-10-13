/*
 * task_fusion.h
 *
 *  Created on: Oct 13, 2025
 *      Author: Serhii Filimoshyn
 */

#ifndef TASKS_TASK_FUSION_H_
#define TASKS_TASK_FUSION_H_

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
//#include "fusion/Fusion.h"
#include "shared/shared_data.h"

void TaskFusion_Init(void);
void TaskFusion(void *pvParameters);

#endif /* TASKS_TASK_FUSION_H_ */
