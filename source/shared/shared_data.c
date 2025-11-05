/*
 * shared_data.c
 *
 *  Created on: Oct 13, 2025
 *      Author: Lenovo
 */

#include "shared_data.h"

ImuSample_t gImuData[2];
volatile uint8_t gImuWriteIndex = 0;
volatile uint8_t gImuReadIndex  = 1;

FusionOutput_t gFusionOutput = {0};
