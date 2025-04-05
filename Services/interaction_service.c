/*
 * input_service.c
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#include <interaction_service.h>
#include "i2c.h"
#include "stm32_timer.h"
#include "sys_app.h"
#include "string.h"

#include "global_config.h"
#include "subghz_phy_app.h"
//#include "radio_service.h"


/* PD ------------------------------------------------------------------*/

#define KEYBOARD_I2C_DELAY 20

// keys
#define ESC_KEY 0x1b
#define BACK_KEY 0x08
#define ENTER_KEY 0x0d

/* Types ------------------------------------------------------------------*/
typedef enum {
    UI_MENU_STATE,
	UI_ENTRY_STATE,
} uiState_t;

/* PV ------------------------------------------------------------------*/

osThreadId_t interactionServiceTaskID = NULL;
uiState_t currentUIState = UI_ENTRY_STATE;

static uint8_t key;
static char inputBuffer[INPUT_BUFFER_SIZE];
static uint8_t inputBufferPosition = 0;

/* PFD ------------------------------------------------------------------*/

static void resetInputState(void);
static void read_keyboard(uint8_t *key);

/* PF ------------------------------------------------------------------*/

static void resetInputState(void) {
	memset(inputBuffer, 0x0, INPUT_BUFFER_SIZE);
	inputBufferPosition = 0;
}

static void read_keyboard(uint8_t *key) {
    if (HAL_I2C_Master_Receive(&hi2c2, 0x5F << 1, key, 1, KEYBOARD_I2C_DELAY) != HAL_OK) {
        *key = 0;  // No key received
    }
}

/* Functions ------------------------------------------------------------------*/

void interactionServiceTask(void *argument) {

	// initialize input buffer
	resetInputState();

	while (1) {
		read_keyboard(&key);

		switch (currentUIState) {
		case UI_MENU_STATE:
			APP_LOG(TS_OFF, VLEVEL_M, "MENU STATE NOW \n\r");
			break;
		case UI_ENTRY_STATE:

			// check for ESC
			if (key == ESC_KEY) {
				resetInputState();
				currentUIState = UI_MENU_STATE;
				break;
			}

			// SEND
			if (key == ENTER_KEY) {
				if (inputBufferPosition > 0 &&
				        inputBufferPosition <= RADIO_MSG_MAX_SIZE &&
				        inputBufferPosition <= INPUT_BUFFER_SIZE) {

						const char msg[] = "Hello world";

				        radioMessage_t pendingRadioMessage;

				        memset(&pendingRadioMessage.data, 0, sizeof(pendingRadioMessage.data));
				        pendingRadioMessage.length = inputBufferPosition;

//				        for (uint16_t i = 0; i < inputBufferPosition; ++i) {
//				            pendingRadioMessage.data[i] = inputBuffer[i];
//				        }
				        memcpy(pendingRadioMessage.data, inputBuffer, strlen(inputBuffer) + 1);
//				        strncpy((char*)pendingRadioMessage.data, msg, strlen(msg));


				        osMessageQueuePut(radioInputQueueHandle, &pendingRadioMessage, 0, 0);

				        resetInputState();
				    } else {
				        APP_LOG(TS_OFF, VLEVEL_M, "[Interaction Service] Invalid input length or empty buffer\n\r");
				    }
				break;
			}

			if (key == BACK_KEY) {
				if (inputBufferPosition != 0) {
					inputBuffer[inputBufferPosition] = 0x0;
					inputBufferPosition--;
					APP_LOG(TS_OFF, VLEVEL_M, "Input Buffer: %s \n\r", inputBuffer);
				}
				break;
			}

			// in entry mode, we build up the buffer
			// valid ASCII
			if (key > 0x19 && key < 0x7F) {

				if (inputBufferPosition < INPUT_BUFFER_SIZE) {
					inputBuffer[inputBufferPosition] = key;
					inputBufferPosition++;
					APP_LOG(TS_OFF, VLEVEL_M, "Input Buffer: %s \n\r", inputBuffer);
				} else {
					// full buffer
					APP_LOG(TS_OFF, VLEVEL_M, "Input buffer full \n\r");
				}
			}

			break;
		}

		osDelay(100);  // Poll every 100 ms
	}
}
