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

/* PD ------------------------------------------------------------------*/

#define KEYBOARD_I2C_DELAY 20

#define INPUT_BUFFER_SIZE 100 // 100 characters

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

uint8_t key;
char inputBuffer[INPUT_BUFFER_SIZE];
uint8_t inputBufferPosition = 0;

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

			// check for ESC or SEND
			if (key == ESC_KEY) {
				resetInputState();
				currentUIState = UI_MENU_STATE;
				break;
			}

			if (key == ENTER_KEY) {
				resetInputState();
				// SEND

				// TEST - print buffer
				APP_LOG(TS_OFF, VLEVEL_M, "Input Buffer to send: %s \n\r", inputBuffer);
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
