/*
 * flexcan0_drv.c
 *
 *  Created on: Oct 13, 2025
 *      Author: Serhii Filimoshyn
 */

#include "flexcan0_drv.h"
#include "shared_data.h"
#include <string.h>

#define FLEXCAN_INSTANCE CAN0

void CAN0_ORed_Message_buffer_IRQHandler(void)
{
    flexcan_frame_t rxFrame;

    for (uint8_t mb = 0; mb < 3; mb++) {
        if (FLEXCAN_GetMbStatusFlags(FLEXCAN_INSTANCE, (1 << mb))) {
            FLEXCAN_ReadRxMb(FLEXCAN_INSTANCE, mb, &rxFrame);
            FLEXCAN_ClearMbStatusFlags(FLEXCAN_INSTANCE, (1 << mb));

            uint32_t id = (rxFrame.id >> FLEXCAN_ID_STD_SHIFT);

            switch (id) {
                case GYRO_MB_ID: // Gyro
                    memcpy(gImuData.gyro, rxFrame.dataByte0, 8);
                    break;
                case ACC_MB_ID: // Accel
                    memcpy(gimuData.accel, rxFrame.dataByte0, 8);
                    break;
                case MAG_MB_ID: // Mag
                    memcpy(gimuData.mag, rxFrame.dataByte0, 8);
                    break;
            }
        }
    }
    SDK_ISR_EXIT_BARRIER;
}




