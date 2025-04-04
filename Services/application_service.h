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

extern osThreadId_t applicationServiceTaskID;

void applicationServiceTask(void *argument);

#endif /* APPLICATION_SERVICE_H_ */
