/*
 * task_fusion.c
 *
 *  Created on: Oct 7, 2025
 *      Author: Serhii Filimoshyn
 */

#include "task_fusion.h"
#include "shared/shared_data.h"
#include "shared/semaphores.h"
#include "drivers/pit_drv.h"
#include "drivers/flexcan1_drv.h"
//#include "fusion/Fusion.h"

//static FusionAhrs ahrs;
//static FusionOffset offset;

void TaskFusion_Init(void) {
//    FusionAhrsInitialise(&ahrs);
//    FusionOffsetInitialise(&offset, 200); // sample rate 200 Hz (5 ms)

    xTaskCreate(TaskFusion, "Fusion_Task", 1024, NULL, tskIDLE_PRIORITY + 4, NULL);
}

/**
 * @brief Runs Fusion algorithm every 5 ms triggered by PIT ISR.
 */
void TaskFusion(void *pvParameters) {
//    FusionVector gyro, acc, mag;
//    FusionEuler euler;
//
//    for (;;) {
//        // Wait for periodic trigger (5 ms)
//        xSemaphoreTake(xSemFusionTick, portMAX_DELAY);
//
//        // Get latest IMU sample
//        xSemaphoreTake(xMutexIMUData, portMAX_DELAY);
//        IMU_Data_t imu = g_imuData;
//        xSemaphoreGive(xMutexIMUData);
//
//        gyro = imu.gyro;
//        acc  = imu.acc;
//        mag  = imu.mag;
//
//        // Update offset + AHRS
//        gyro = FusionOffsetUpdate(&offset, gyro);
//        FusionAhrsUpdate(&ahrs, gyro, acc, mag, 0.005f); // 5 ms
//
//        euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
//
//        // Save result
//        xSemaphoreTake(xMutexFusionData, portMAX_DELAY);
//        g_fusionOutput.euler = euler;
//        xSemaphoreGive(xMutexFusionData);
//
//        // Notify CANopen task to send
//        xSemaphoreGive(xSemFusionResultReady);
//    }

	for (;;) {

	}

}


