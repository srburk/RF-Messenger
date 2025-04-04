/*
 * radio_service.c
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#include "radio_service.h"

#include "stm32_timer.h"
#include "sys_app.h"

void radioServiceTask(void *argument) {
    for (;;) {
        // Perform service operations here
		APP_LOG(TS_ON, VLEVEL_M, "Radio Service\n\r");
        osDelay(1000); // Delay for 1000 milliseconds
    }
}
