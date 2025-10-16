/*
 * Copyright 2016-2025 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file    kfx_backpan_fusion.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"

/* TODO: insert other include files here. */
#include "drivers/flexcan0_drv.h"
#include "drivers/flexcan1_drv.h"
#include "drivers/pit_drv.h"

#include "shared/semaphores.h"
#include "shared/shared_data.h"

#include "tasks/task_imu.h"
#include "tasks/task_fusion.h"
#include "tasks/task_canopen.h"

#include "app_init.h"
/* TODO: insert other definitions and declarations here. */

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    PRINTF("Hello World\r\n");

    /* FreeRTOS Initialization */

    /* Semaphores Initialization */
    Semaphores_Init();

    /* Tasks Initialization */

    /* IMU Data ReceiveTask Initialization */
    TaskIMU_Init();

    /* FusionTask Initialization */
    TaskFusion_Init();

    /* CANopen Task Initialization */
    TaskCANopen_Init();

    /* Start Scheduling */
    vTaskStartScheduler();

    /* Force the counter to be placed into memory. */
    volatile static int i = 0 ;
    /* Enter an infinite loop, just incrementing a counter. */
    while(1) {
        i++ ;
        /* 'Dummy' NOP to allow source level single stepping of
            tight while() loop */
        __asm volatile ("nop");
    }
    return 0 ;
}
