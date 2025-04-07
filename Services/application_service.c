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

/* PD ------------------------------------------------------------------*/

#define ISR_FLAG_TIMEOUT 10

/* Public Variables ------------------------------------------------------------------*/

osThreadId_t applicationServiceTaskID = NULL;
osEventFlagsId_t applicationEventsHandle;

/* Private variables ------------------------------------------------------------------*/

static appState_t appState = APP_IDLE;

/* Functions ------------------------------------------------------------------*/

void applicationServiceTask(void *argument) {

	for (;;) {
//		uint32_t result = osThreadFlagsWait(applicationEventsHandle, osFlagsWaitAny, ISR_FLAG_TIMEOUT);

//		if (result & flags) {
			// Handle the event triggered by the ISR
//			APP_LOG(TS_OFF, VLEVEL_M, " [app_service] Event received by application! \n\r");
//			break;
//		}

		switch (appState) {
		case APP_IDLE:



			break;
		case APP_LISTENING:
			break;
		}
	}
}
