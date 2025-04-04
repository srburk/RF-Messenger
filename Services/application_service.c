/*
 * application_service.c
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#include "application_service.h"

#include "stm32_timer.h"
#include "sys_app.h"

// Private Define

#define ISR_FLAG_TIMEOUT 50

// Variables

osThreadId_t applicationServiceTaskID = NULL;

// Keyboard
uint8_t key;

// Private Function Definitions

static void inputSubTask(void);

// Private Functions

static void inputSubTask(void) {

}

// Public Functions

void applicationServiceTask(void *argument) {
	uint32_t flags = 0x00000001U;

	for (;;) {
		uint32_t result = osThreadFlagsWait(flags, osFlagsWaitAny, ISR_FLAG_TIMEOUT);

		if (result & flags) {
			// Handle the event triggered by the ISR
			APP_LOG(TS_OFF, VLEVEL_M, "Event received by application! \n\r");
		}

	}
}
