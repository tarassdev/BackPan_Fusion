/*
 * semaphores.c
 *
 *  Created on: Oct 13, 2025
 *      Author: Lenovo
 */

#include "semaphores.h"

SemaphoreHandle_t xImuDataReady;
SemaphoreHandle_t xFusionDone;
SemaphoreHandle_t xFusionOutputMutex;
QueueHandle_t can0RxQueue = NULL;

void Semaphores_Init(void)
{
	xImuDataReady = xSemaphoreCreateBinary();
	xFusionDone = xSemaphoreCreateBinary();
	xFusionOutputMutex = xSemaphoreCreateMutex();
	/* queue element size should fit CAN msg (id + up to 8 bytes) */
	can0RxQueue = xQueueCreate(32, sizeof(uint64_t) /* or a struct for id+len+data */);
}
