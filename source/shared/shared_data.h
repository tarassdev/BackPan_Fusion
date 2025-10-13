/*
 * shared_data.h
 *
 *  Created on: Oct 13, 2025
 *      Author: Lenovo
 */

#ifndef SHARED_SHARED_DATA_H_
#define SHARED_SHARED_DATA_H_

#pragma once
#include "FreeRTOS.h"
#include "semphr.h"

typedef struct {
	float accel[3];
	float gyro[3];
	float mag[3];
	TickType_t timestamp;
} ImuSample_t;

typedef struct {
	float roll, pitch, yaw;
	TickType_t timestamp;
} FusionOutput_t;

extern ImuSample_t gImuData;
extern FusionOutput_t gFusionData;

#endif /* SHARED_SHARED_DATA_H_ */
