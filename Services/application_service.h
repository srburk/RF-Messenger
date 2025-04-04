/*
 * application_service.h
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#ifndef APPLICATION_SERVICE_H_
#define APPLICATION_SERVICE_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"

/* Defines ------------------------------------------------------------------*/
#define KEY_EVENT_FLAG	0x01
#define RADIO_EVENT_FLAG 0x02

/* Variables ------------------------------------------------------------------*/

extern osThreadId_t applicationServiceTaskID;

/* Functions ------------------------------------------------------------------*/

void applicationServiceTask(void *argument);

#endif /* APPLICATION_SERVICE_H_ */
