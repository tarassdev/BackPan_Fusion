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



void TaskFusion_Init(void) {

    xTaskCreate(TaskFusion, "Fusion_Task", 1024, NULL, tskIDLE_PRIORITY + 4, NULL);
}

/**
 * @brief Runs Fusion algorithm every 5 ms triggered by PIT ISR.
 */
void TaskFusion(void *pvParameters) {

	for (;;) {

	}

}


