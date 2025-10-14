/*
 * flexcan0_drv.h
 *
 *  Created on: Oct 13, 2025
 *      Author: Serhii Filimoshyn
 */

#ifndef DRIVERS_FLEXCAN0_DRV_H_
#define DRIVERS_FLEXCAN0_DRV_H_

#include "MKV56F24.h"
#include "fsl_flexcan.h"

#define GYRO_MB_ID 0x32
#define ACC_MB_ID 0x34
#define MAG_MB_ID 0x41

void CAN0_ORed_Message_buffer_IRQHandler(void);


#endif /* DRIVERS_FLEXCAN0_DRV_H_ */
