/*
 * input_service.h
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#ifndef INTERACTION_SERVICE_H_
#define INTERACTION_SERVICE_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"

extern osThreadId_t interactionServiceTaskID;

void interactionServiceTask(void *argument);

#endif /* INTERACTION_SERVICE_H_ */
