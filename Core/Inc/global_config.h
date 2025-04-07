/*
 * global_config.h
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#ifndef INC_GLOBAL_CONFIG_H_
#define INC_GLOBAL_CONFIG_H_

// global application buffer sizes that should be standard
#define APP_BUFFER_SIZE				100
#define RADIO_MSG_MAX_SIZE			APP_BUFFER_SIZE
#define INPUT_BUFFER_SIZE			APP_BUFFER_SIZE

// message buffers
#define RADIO_SEND_QUEUE_SIZE		4 // max message queue for send
#define RADIO_RECEIVE_QUEUE_SIZE	4 // max queue for receive (before going into cold storage or other

// events

#define EVENT_USER_RX_MODE			(0x1 << 0) // user is listening
#define EVENT_USER_IDLE				(0x1 << 1) // user is in idle (menu or send)

#endif /* INC_GLOBAL_CONFIG_H_ */
