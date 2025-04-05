/*
 * radio_service.h
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#ifndef RADIO_SERVICE_H_
#define RADIO_SERVICE_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"

#include "app_subghz_phy.h"

#include "global_config.h"

/* Types ------------------------------------------------------------------*/

typedef struct {
	uint8_t data[RADIO_MSG_MAX_SIZE];
	uint8_t length;
} radioMessage_t;

/* Exported Variables ------------------------------------------------------------------*/

extern osMessageQueueId_t radioInputQueueHandle;

/* Functions ------------------------------------------------------------------*/

void radioServiceTask(void *argument);

#endif /* RADIO_SERVICE_H_ */
