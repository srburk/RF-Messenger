/*
 * input_service.h
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#ifndef INPUT_SERVICE_H_
#define INPUT_SERVICE_H_

/* Includes ------------------------------------------------------------------*/
#include "cmsis_os.h"

extern osThreadId_t inputServiceTaskID;

void inputServiceTask(void *argument);

#endif /* INPUT_SERVICE_H_ */
