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
#include "FusionMath.h"

typedef struct {
	FusionVector gyro;
	FusionVector acc;
	FusionVector mag;
	TickType_t timestamp;
} ImuSample_t;

typedef struct {
	float roll, pitch, yaw;
	TickType_t timestamp;
} FusionOutput_t;

extern ImuSample_t gImuData;
extern FusionOutput_t gFusionData;

#endif /* SHARED_SHARED_DATA_H_ */
