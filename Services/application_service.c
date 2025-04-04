/*
 * application_service.c
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#include "application_service.h"

#include "i2c.h"
#include "stm32_timer.h"
#include "sys_app.h"

// Private Define

#define ISR_FLAG_TIMEOUT 10

// Variables

osThreadId_t applicationServiceTaskID = NULL;

// Public Functions

void applicationServiceTask(void *argument) {
	uint32_t flags = 0x00000001U;

	for (;;) {
		uint32_t result = osThreadFlagsWait(flags, osFlagsWaitAny, ISR_FLAG_TIMEOUT);

		if (result & flags) {
			// Handle the event triggered by the ISR
			APP_LOG(TS_OFF, VLEVEL_M, " [app_service] Event received by application! \n\r");
		}
	}
}
