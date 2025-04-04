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

void radioServiceTask(void *argument);

#endif /* RADIO_SERVICE_H_ */
