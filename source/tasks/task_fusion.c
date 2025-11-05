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
#include "fusion/Fusion.h"


TaskHandle_t fusionTaskHandle = NULL;

#define FUSION_STACK_SIZE  	(1024)
#define FUSION_PRIORITY    	(configMAX_PRIORITIES - 3)
#define FUSION_DT_SECONDS  	(0.005f)
#define SAMPLE_RATE 		(100)

static FusionOffset offset;
static FusionAhrs ahrs;

// Define calibration
static const FusionMatrix gyroscopeMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
static const FusionVector gyroscopeSensitivity = {1.0f, 1.0f, 1.0f};
static const FusionVector gyroscopeOffset = {0.0f, 0.0f, 0.0f};
static const FusionMatrix accelerometerMisalignment = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
static const FusionVector accelerometerSensitivity = {1.0f, 1.0f, 1.0f};
static const FusionVector accelerometerOffset = {0.0f, 0.0f, 0.0f};
static const FusionMatrix softIronMatrix = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
static const FusionVector hardIronOffset = {0.0f, 0.0f, 0.0f};

void TaskFusion_Init(void) {

	// Initialise algorithms
	FusionOffsetInitialise(&offset, SAMPLE_RATE);
	FusionAhrsInitialise(&ahrs);

	// Set AHRS algorithm settings
	static const FusionAhrsSettings settings = {
			.convention = FusionConventionNwu,
			.gain = 0.5f,
			.gyroscopeRange = 2000.0f, /* replace this with actual gyroscope range in degrees/s */
			.accelerationRejection = 10.0f,
			.magneticRejection = 10.0f,
			.recoveryTriggerPeriod = 5 * SAMPLE_RATE, /* 5 seconds */
	};
	FusionAhrsSetSettings(&ahrs, &settings);

	xTaskCreate(TaskFusion, "Fusion_Task", FUSION_STACK_SIZE, NULL, FUSION_PRIORITY, NULL);
}

/**
 * @brief Runs Fusion algorithm every 5 ms triggered by PIT ISR.
 */
void TaskFusion(void *pvParameters) {

	for (;;)
	{
		/* Wait for PIT (5 ms). Using notification means PIT will call vTaskNotifyGiveFromISR */
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		/* Try to get newest IMU buffer - check semaphore without blocking because fusion runs every 5ms regardless */
		ImuSample_t localSample = {0};
		BaseType_t haveSample = pdFALSE;
		if (xImuDataReady != NULL) {
			/* non-blocking attempt: take if a sample is available */
			if (xSemaphoreTake(xImuDataReady, 0) == pdTRUE) {
				/* copy active read buffer under critical section to avoid toggling race */
				taskENTER_CRITICAL();
				localSample = gImuData[gImuReadIndex];
				taskEXIT_CRITICAL();
				haveSample = pdTRUE;
			}
		} else {
			/* If no semaphore used, just copy current read index */
			taskENTER_CRITICAL();
			localSample = gImuData[gImuReadIndex];
			taskEXIT_CRITICAL();
			haveSample = pdTRUE;
		}

		if (haveSample) {
			/* convert to FusionVector and run AHRS. Assume incoming units match Fusion expectations */
			FusionVector gyro = localSample.gyro;
			FusionVector acc = localSample.acc;
			FusionVector mag = localSample.mag;

			/* apply calibration */
			gyro = FusionCalibrationInertial(gyro, gyroscopeMisalignment, gyroscopeSensitivity, gyroscopeOffset);
			acc	 = FusionCalibrationInertial(acc, accelerometerMisalignment, accelerometerSensitivity, accelerometerOffset);
			mag	 = FusionCalibrationMagnetic(mag, softIronMatrix, hardIronOffset);


			/* apply gyro offset correction */
			gyro = FusionOffsetUpdate(&offset, gyro);

			/* choose update depending on magnetometer presence */
			if ((localSample.flags & 0x04U) != 0U) {
				FusionAhrsUpdate(&ahrs, gyro, acc, mag, FUSION_DT_SECONDS);
			} else {
				FusionAhrsUpdateNoMagnetometer(&ahrs, gyro, acc, FUSION_DT_SECONDS);
			}

			/* store result protected by mutex */
			if (xFusionOutputMutex != NULL) {
				if (xSemaphoreTake(xFusionOutputMutex, pdMS_TO_TICKS(2)) == pdTRUE) {
					//	gFusionOutput.timestamp_us = localSample.timestamp_us;
					gFusionOutput.quat = FusionAhrsGetQuaternion(&ahrs);
					gFusionOutput.euler = FusionQuaternionToEuler(gFusionOutput.quat);
					gFusionOutput.earth = FusionAhrsGetEarthAcceleration(&ahrs);
					xSemaphoreGive(xFusionOutputMutex);
				}
			} else {
				//	            	gFusionOutput.timestamp_us = localSample.timestamp_us;
				gFusionOutput.quat = FusionAhrsGetQuaternion(&ahrs);
				gFusionOutput.euler = FusionQuaternionToEuler(gFusionOutput.quat);
				gFusionOutput.earth = FusionAhrsGetEarthAcceleration(&ahrs);
			}
			/* optionally signal CANopen task by semaphore (not shown) */
		} else {
			/* no fresh IMU data this cycle — integrate using last gyro or skip */
			/* Optionally call FusionAhrsUpdateNoMagnetometer with zero accel/mag if required */
		}
	} /* for */

}


