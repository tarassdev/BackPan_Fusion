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
#include "shared/shared_data.h"

void TaskIMU_Init(void);
void TaskIMU(void *pvParameters);

void Decode_RateOfTurnHR(uint8_t *data, FusionVector *gyro);
void Decode_AccelerationHR(uint8_t *data, FusionVector *acc);
void Decode_MagneticField(uint8_t *data, FusionVector *mag);


#endif /* TASKS_TASK_IMU_H_ */
