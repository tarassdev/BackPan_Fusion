/*
 * task_canopen.c
 *
 *  Created on: Oct 7, 2025
 *      Author: Serhii Filimoshyn
 */

#include "task_canopen.h"
#include "shared/shared_data.h"
#include "shared/semaphores.h"

void TaskCANopen_Init(void) {
    xTaskCreate(TaskCANopen, "CANopen_Task", 512, NULL, 4, NULL);
}

/**
 * @brief Sends Fusion results immediately when ready.
 */
void TaskCANopen(void *pvParameters) {
//    for (;;) {
//        if (xSemaphoreTake(xSemFusionResultReady, portMAX_DELAY) == pdTRUE) {
//            xSemaphoreTake(xMutexFusionData, portMAX_DELAY);
//            FusionOutput_t data = g_fusionOutput;
//            xSemaphoreGive(xMutexFusionData);
//
//            CAN_Message_t tx = {0};
//            tx.id = 0x180; // Example PDO ID
//            memcpy(tx.data, &data.euler, sizeof(data.euler));
//            tx.len = 8;
//
//            FLEXCAN1_SendFrame(&tx);
//        }
//    }
	for (;;) {

	}

}
