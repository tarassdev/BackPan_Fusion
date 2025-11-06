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

/*
 * Task: CANopen SYNC producer + TPDO trigger
 *
 * Assumptions:
 *  - Global CANopenNode context pointer: CO_t *CO (extern)
 *  - CANopenNode compiled and linked; CO is created by CANopen init code.
 *  - OD_quat_* variables are mapped into the OD so TPDO1 maps them.
 *  - xSemFusionResultReady binary semaphore is created in Semaphores_Init.
 *
 * Behavior:
 *  - Periodically calls CO_process(CO, false, 1000)
 *  - When xSemFusionResultReady is given (by task_fusion), send SYNC and process TPDOs so TPDOs transmit after SYNC.
 */

//#include "task_canopen.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "semphr.h"
//#include "od/OD.h"
//#include "shared/semaphores.h"
//#include "shared/shared_data.h"
//#include "CO_driver.h"
//#include "CO_ODinterface.h"
//#include "CO_PDO.h"
//#include "CO_SDO.h"
//#include "CO_NMT_Heartbeat.h"
//#include "CO_SYNC.h"
//#include "CO_SDO.h"
//#include "CO.h"
//
//#include <stdint.h>
//
//extern CO_t *CO; /* provided by CANopenNode initialization code */
//
//#define CANOPEN_TASK_PERIOD_MS   1U   /* run CO_process every 1 ms */
//#define CANOPEN_TASK_STACK_WORDS 1024
//#define CANOPEN_TASK_PRIORITY    (configMAX_PRIORITIES - 4)
//
//static void vTaskCANopen(void *pvParameters)
//{
//    TickType_t xLastWakeTime = xTaskGetTickCount();
//    const TickType_t xPeriod = pdMS_TO_TICKS(CANOPEN_TASK_PERIOD_MS);
//
//    for (;;) {
//        /* Stack housekeeping: pass elapsed time in microseconds (here 1000 us) */
//        CO_process(CO, false, 1000U);
//
//        /* If fusion has posted a fresh result, send SYNC and trigger TPDOs */
//        if (xSemTake(xSemFusionResultReady, 0) == pdTRUE) {
//            /* Ensure node is operational or pre-operational if allowed (the stack may expose helper) */
//            if (CO != NULL && CO->SYNC != NULL) {
//
//                /* Explicitly send SYNC */
//                CO_SYNCsend(CO->SYNC);
//
//                /* Immediately process TPDOs to force transmissions */
//                /* Number of TPDOs and access method depends on stack version / configuration */
//                /* Typical access: CO->TPDO[] is an array of tpdo objects */
//                /* attempt to process first N TPDOs present */
//#ifdef CO_TPDO_MODULE
//                /* Example: if CO provides CO->TPDO and TPDO count */
//                for (uint8_t i = 0; i < CO->TPDO->objNrOf; ++i) {
//                    CO_TPDO_send(CO->TPDO + i); /* some stacks offer CO_TPDO_send */
//                }
//#else
//                /* Fallback: call CO_TPDO_process for each known TPDO index (0..count-1)
//                  must adapt this section to your CANopenNode build. */
//                /* Example: if  CO object has member TPDOCount or similar, use it */
//#endif
//            }
//        }
//
//        vTaskDelayUntil(&xLastWakeTime, xPeriod);
//    }
//}
//
//void TaskCANopen_Create(void)
//{
//    xTaskCreate(vTaskCANopen, "CANopenSYNC", CANOPEN_TASK_STACK_WORDS, NULL, CANOPEN_TASK_PRIORITY, NULL);
//}
//
