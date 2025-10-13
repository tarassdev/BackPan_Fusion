/*
 * task_imu.c
 *
 *  Created on: Oct 13, 2025
 *      Author: Serhii Filimoshyn
 */

#include "task_imu.h"
#include "shared/shared_data.h"
#include "shared/semaphores.h"
#include <string.h>

void TaskIMU_Init(void) {
	xTaskCreate(TaskIMU, "IMU_Task", 512, NULL, 3, NULL);
}

void TaskIMU(void *pvParameters) {

//	CAN_Message_t msg;
//
//	for (;;) {
//		if (xQueueReceive(xQueueIMUCanRx, &msg, portMAX_DELAY) == pdTRUE) {
//			// Decode IMU data (replace with real conversion)
//			IMU_Data_t imu;
//			memcpy(&imu.raw[0], msg.data, sizeof(msg.data));
//
//			xSemaphoreTake(xMutexIMUData, portMAX_DELAY);
//			g_imuData = imu;
//			xSemaphoreGive(xMutexIMUData);
//		}
//	}

	for (;;) {

	}

}
