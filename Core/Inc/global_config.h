/*
 * global_config.h
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#ifndef INC_GLOBAL_CONFIG_H_
#define INC_GLOBAL_CONFIG_H_

// global application buffer sizes that should be standard
#define APP_BUFFER_SIZE			100
#define RADIO_MSG_MAX_SIZE		APP_BUFFER_SIZE
#define INPUT_BUFFER_SIZE		APP_BUFFER_SIZE

// message buffers
#define RADIO_INPUT_QUEUE_SIZE	4 // max message queue for send

#endif /* INC_GLOBAL_CONFIG_H_ */
