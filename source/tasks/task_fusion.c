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

TaskHandle_t fusionTaskHandle = NULL;

#define FUSION_STACK_SIZE  (1024)
#define FUSION_PRIORITY    (configMAX_PRIORITIES - 3)
#define FUSION_DT_SECONDS  (0.005f)


void TaskFusion_Init(void) {
    xTaskCreate(TaskFusion, "Fusion_Task", FUSION_STACK_SIZE, NULL, FUSION_PRIORITY, NULL);
}

/**
 * @brief Runs Fusion algorithm every 5 ms triggered by PIT ISR.
 */
void TaskFusion(void *pvParameters) {

	for (;;) {

	}

}


