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
#include "../fusion/FusionMath.h"

typedef struct {
	FusionVector gyro;
	FusionVector acc;
	FusionVector mag;
	TickType_t timestamp;
	uint8_t flags;
} ImuSample_t;

typedef struct {
	float roll, pitch, yaw;
	TickType_t timestamp;
	FusionQuaternion quat;
	FusionEuler euler;
	FusionVector earth;
} FusionOutput_t;

extern ImuSample_t gImuData[2];
extern volatile uint8_t gImuWriteIndex; /* toggled by producer */
extern volatile uint8_t gImuReadIndex;  /* used by consumer */


typedef struct {
	uint32_t id;
	uint8_t data[8];
	uint32_t len;
} CAN_Rx_t;

extern FusionOutput_t gFusionOutput;

#endif /* SHARED_SHARED_DATA_H_ */
