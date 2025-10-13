/*
 * semaphores.h
 *
 *  Created on: Oct 13, 2025
 *      Author: Lenovo
 */

#ifndef SHARED_SEMAPHORES_H_
#define SHARED_SEMAPHORES_H_

#pragma once
#include "FreeRTOS.h"
#include "semphr.h"

extern SemaphoreHandle_t xImuDataReady; 		//	IMU -> Fusion algorithm
extern SemaphoreHandle_t xFusionDone; 			//	Fusion -> CANOpen
extern SemaphoreHandle_t xSharedDataMutex;		//	 protects shared data

void Semaphores_Init(void);

#endif /* SHARED_SEMAPHORES_H_ */
