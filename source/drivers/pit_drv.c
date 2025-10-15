/*
 * pit_drv.c
 *
 *  Created on: Oct 13, 2025
 *      Author: Serhii Filimoshyn
 */
#include "pit_drv.h"

void PIT0_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);

	if(fusionTaskHandle != NULL)
		vTaskNotifyGiveFromISR(fusionTaskHandle, &xHigherPriorityTaskWoken);

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	SDK_ISR_EXIT_BARRIER;
}

