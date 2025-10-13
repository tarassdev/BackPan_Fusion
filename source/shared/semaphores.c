/*
 * semaphores.c
 *
 *  Created on: Oct 13, 2025
 *      Author: Lenovo
 */

#include "semaphores.h"

SemaphoreHandle_t xImuDataReady;
SemaphoreHandle_t xFusionDone;
SemaphoreHandle_t xSharedDataMutex;

void Semaphores_Init(void)
{
	xImuDataReady = xSemaphoreCreateBinary();
	xFusionDone = xSemaphoreCreateBinary();
	xSharedDataMutex = xSemaphoreCreateMutex();
}
