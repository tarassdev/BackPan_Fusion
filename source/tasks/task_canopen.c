/*
 * task_canopen.c
 *
 *  Created on: Oct 7, 2025
 *      Author: Serhii Filimoshyn
 */

#include "task_canopen.h"
#include "shared/shared_data.h"
#include "shared/semaphores.h"
#include <string.h>

typedef struct {
	uint32_t id;
	uint8_t data[8];
	uint32_t len;
} CAN_Tx_t;

void TaskCANopen_Init(void) {
    xTaskCreate(TaskCANopen, "CANopen_Task", 512, NULL, 4, NULL);
}

/**
 * @brief Sends Fusion results immediately when ready.
 */
void TaskCANopen(void *pvParameters) {
    for (;;) {
        if (xSemaphoreTake(xFusionOutputMutex, portMAX_DELAY) == pdTRUE) {
            FusionOutput_t CANOutputData = gFusionOutput;
            xSemaphoreGive(xFusionOutputMutex);

            CAN_Tx_t tx = {0};
            tx.id = 0x180; // Example PDO ID
            memcpy(tx.data, &CANOutputData.euler, sizeof(CANOutputData.euler));
            tx.len = 8;

//            FLEXCAN1_SendFrame(&tx);
        }
    }
}
