/*
 * radio_service.c
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#include "radio_service.h"

#include "stm32_timer.h"
#include "sys_app.h"

osMessageQueueId_t radioInputQueueHandle;

void radioServiceTask(void *argument) {

	// init radio service

	radioMessage_t incomingMessage;

    for (;;) {
        // TODO: Add more sophisticated radio state machine and handling for messages
    	if (osMessageQueueGet(radioInputQueueHandle, &incomingMessage, NULL, osWaitForever) == osOK) {
			// Send the data
			APP_LOG(TS_ON, VLEVEL_M, "Got message: %s \n\r", incomingMessage);
		}
    }
}
