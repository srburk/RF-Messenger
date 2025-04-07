/*
 * application_service.h
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#ifndef APPLICATION_SERVICE_H_
#define APPLICATION_SERVICE_H_

/*
 *
 *  Application Service is responsible for application state, managing persistence, and sending signals for entering low power states
 *  There should be some global state that translates to sub-states for services.
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"

/* Defines ------------------------------------------------------------------*/
#define KEY_EVENT_FLAG	0x01
#define RADIO_EVENT_FLAG 0x02

/* Types ------------------------------------------------------------------*/

typedef enum
{
  APP_IDLE,
  APP_LISTENING,
} appState_t;

/* Variables ------------------------------------------------------------------*/

extern osThreadId_t applicationServiceTaskID;

// event handle
extern osEventFlagsId_t applicationEventsHandle;

/* Functions ------------------------------------------------------------------*/

void applicationServiceTask(void *argument);

#endif /* APPLICATION_SERVICE_H_ */
