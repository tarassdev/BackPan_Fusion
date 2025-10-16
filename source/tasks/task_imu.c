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

#define GYRO_RESOLUTION 1.0f/256.0f
#define ACC_RESOLUTION 	1.0f/256.0f
#define MAG_RESOLUTION 	1.0f/256.0f

#define SWAP_INT16(x) (((x) >> 8) | ((x) << 8))
#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24))

#define FULL_DATA_MASK  (0x01U | 0x02U | 0x04U) /* gyro | accel | mag */

void TaskIMU_Init(void) {
	xTaskCreate(TaskIMU, "IMU_Task", 512, NULL, 3, NULL);
}

void TaskIMU(void *pvParameters) {

	CAN_Rx_t rx;
	ImuSample_t *writeBuf;

	for (;;) {
		if (xQueueReceive(can0RxQueue, &rx, portMAX_DELAY) == pdTRUE) {
			writeBuf = &gImuData[gImuWriteIndex];

			/* decode frame into writeBuf depending on rx.id */
			if (rx.id == 0x62) { /* RateOfTurnHR (gyro) */
				Decode_RateOfTurnHR(rx.data, &writeBuf->gyro);
				writeBuf->flags |= 0x01U;
			} else if (rx.id == 0x61) { /* AccelerationHR */
				Decode_AccelerationHR(rx.data, &writeBuf->acc);
				writeBuf->flags |= 0x02U;
			} else if (rx.id == 0x41) { /* MagneticField */
				Decode_MagneticField(rx.data, &writeBuf->mag);
				writeBuf->flags |= 0x04U;
			}

			/* timestamp */
//			writeBuf->timestamp_us = PIT_GetTimestampUs();

			/* When the write buffer contains a full set (or policy satisfied) */
			if ((writeBuf->flags & FULL_DATA_MASK) == FULL_DATA_MASK) {
				/* swap read index and write index atomically */
				taskENTER_CRITICAL();
				gImuReadIndex = gImuWriteIndex;
				gImuWriteIndex ^= 1; /* toggle */
				/* clear the new write buffer flags so builder writes fresh next time */
				gImuData[gImuWriteIndex].flags = 0;
				taskEXIT_CRITICAL();

				/* notify fusion task: give binary semaphore (or just give flag) */
				if (xImuDataReady != NULL) {
					xSemaphoreGive(xImuDataReady);
				}
			}
		}
	}
}

void Decode_RateOfTurnHR(uint8_t *data, FusionVector *gyro)
{
	gyro->axis.x = (float)(GYRO_RESOLUTION * (((uint16_t)data[1]) << 8 | (uint16_t)data[0]));
	gyro->axis.y = (float)(GYRO_RESOLUTION * (((uint16_t)data[3]) << 8 | (uint16_t)data[2]));
	gyro->axis.z = (float)(GYRO_RESOLUTION * (((uint16_t)data[5]) << 8 | (uint16_t)data[4]));
}

void Decode_AccelerationHR(uint8_t *data, FusionVector *acc)
{
	acc->axis.x = (float)(ACC_RESOLUTION * (((uint16_t)data[1]) << 8 | (uint16_t)data[0]));
	acc->axis.y = (float)(ACC_RESOLUTION * (((uint16_t)data[3]) << 8 | (uint16_t)data[2]));
	acc->axis.z = (float)(ACC_RESOLUTION * (((uint16_t)data[5]) << 8 | (uint16_t)data[4]));
}

void Decode_MagneticField(uint8_t *data, FusionVector *mag)
{
	mag->axis.x = (float)(MAG_RESOLUTION * (((uint16_t)data[1]) << 8 | (uint16_t)data[0]));
	mag->axis.y = (float)(MAG_RESOLUTION * (((uint16_t)data[3]) << 8 | (uint16_t)data[2]));
	mag->axis.z = (float)(MAG_RESOLUTION * (((uint16_t)data[5]) << 8 | (uint16_t)data[4]));
}
