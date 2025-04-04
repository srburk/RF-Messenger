/*
 * application_service.c
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#include "application_service.h"

#include "stm32_timer.h"
#include "sys_app.h"

osThreadId_t applicationServiceTaskID = NULL;

void applicationServiceTask(void *argument) {
	uint32_t flags = 0x00000001U;

	for (;;) {
		uint32_t result = osThreadFlagsWait(flags, osFlagsWaitAny, osWaitForever);
		if (result & flags) {
			// Handle the event triggered by the ISR
			APP_LOG(TS_OFF, VLEVEL_M, "Event received by application! \n\r");
		} else {
			// Handle error or timeout
			APP_LOG(TS_OFF, VLEVEL_H, "False alarm on event in application service\n\r");
		}
	}
}
