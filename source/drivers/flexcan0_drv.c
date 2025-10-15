/*
 * flexcan0_drv.c
 *
 *  Created on: Oct 13, 2025
 *      Author: Serhii Filimoshyn
 */

#include "flexcan0_drv.h"
#include "../shared/shared_data.h"
#include <string.h>
#include "../shared/semaphores.h"

#define FLEXCAN_INSTANCE0 CAN0
#define FLEXCAN_ID_STD_SHIFT 18

void CAN0_ORed_Message_buffer_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    flexcan_frame_t rxFrame;

    for (uint8_t mb = 0; mb < 3; mb++) {
        if (FLEXCAN_GetMbStatusFlags(FLEXCAN_INSTANCE0, (1 << mb))) {
            FLEXCAN_ReadRxMb(FLEXCAN_INSTANCE0, mb, &rxFrame);
            FLEXCAN_ClearMbStatusFlags(FLEXCAN_INSTANCE0, (1 << mb));

            uint32_t id = (rxFrame.id >> FLEXCAN_ID_STD_SHIFT);

            CAN_Rx_t rx;
            rx.id = id;
            rx.len = rxFrame.length;
            memcpy(rx.data, &rxFrame.dataByte0, rx.len);

            /* push to queue (drop oldest if queue full) */
            if (xQueueSendFromISR(can0RxQueue, &rx, &xHigherPriorityTaskWoken) != pdTRUE) {
            	CAN_Rx_t tmp;
            	(void)xQueueReceiveFromISR(can0RxQueue, &tmp, NULL); /* drop oldest */
            	(void)xQueueSendFromISR(can0RxQueue, &rx, &xHigherPriorityTaskWoken);
            }
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    SDK_ISR_EXIT_BARRIER;
}




