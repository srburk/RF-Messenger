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
#include "liquidcrystal_i2c.h"

#include "global_config.h"
#include "subghz_phy_app.h"
#include "subghz_phy_app.h"
//#include "radio_service.h"


/* PD ------------------------------------------------------------------*/

#define KEYBOARD_I2C_DELAY 20
#define LCD_DISPLAY_WIDTH 20

// keys
#define ESC_KEY 0x1b
#define BACK_KEY 0x08
#define ENTER_KEY 0x0d
#define LEFT_ARROW 0xb4
#define RIGHT_ARROW 0xb7

/* Types ------------------------------------------------------------------*/
typedef enum {
    UI_MENU_STATE,
	UI_ENTRY_STATE,
	UI_RECEIVE_STATE
} uiState_t;

/* PV ------------------------------------------------------------------*/

osThreadId_t interactionServiceTaskID = NULL;
uiState_t currentUIState = UI_MENU_STATE;

static uint8_t key;
static char messageBuffer[INPUT_BUFFER_SIZE];
static uint8_t messageBufferPosition = 0;

static const char menuTopLeftText[] = "SEND";
static const char menuTopRightText[] = "RECEIVE";
static const char menuBottomText[] = "<- SELECTION ->";

//static const char entryText[] = "ENTER:";

//static const char receivedText[] = "RECEIVED:";

/* PFD ------------------------------------------------------------------*/

static void resetMessageBuffer(void);
static void readKeyboard(uint8_t *key);
static void checkForReturnToMenu(uint8_t *key);

/* PF ------------------------------------------------------------------*/

static void resetMessageBuffer(void) {
	memset(messageBuffer, 0x0, INPUT_BUFFER_SIZE);
	messageBufferPosition = 0;
}

static void readKeyboard(uint8_t *key) {
    if (HAL_I2C_Master_Receive(&hi2c2, 0x5F << 1, key, 1, KEYBOARD_I2C_DELAY) != HAL_OK) {
        *key = 0;  // No key received
    }
}

static void checkForReturnToMenu(uint8_t *key) {
	if (*key == ESC_KEY) {
		resetMessageBuffer();
		HD44780_Clear();
		HD44780_SetCursor(0,0);
		currentUIState = UI_MENU_STATE;
		resetMessageBuffer();

		// let the radio know to cool off
		osThreadFlagsSet(radioServiceTaskID, EVENT_USER_IDLE);
	}
}

/* Functions ------------------------------------------------------------------*/

void interactionServiceTask(void *argument) {

	// initialize input buffer
	resetMessageBuffer();

	// initialize the LCD screen
	HD44780_Init(4);
	HD44780_Blink();
	HD44780_Clear();

	while (1) {
		readKeyboard(&key);

		switch (currentUIState) {
		case UI_MENU_STATE:

			// print user menu to the LCD screen
			HD44780_SetCursor(0, 0);
			HD44780_PrintStr(menuTopLeftText);
			HD44780_SetCursor(LCD_DISPLAY_WIDTH - strlen(menuTopRightText),0);
			HD44780_PrintStr(menuTopRightText);
			HD44780_SetCursor((LCD_DISPLAY_WIDTH - strlen(menuBottomText)) / 2, 3);
			HD44780_PrintStr(menuBottomText);

			// check for user menu selection
			if (key == LEFT_ARROW) {
				HD44780_Clear();
				resetMessageBuffer();
				currentUIState = UI_ENTRY_STATE;
				break;
			}

			if (key == RIGHT_ARROW) {
				HD44780_Clear();
				resetMessageBuffer();
				currentUIState = UI_RECEIVE_STATE;

				// let application know we're listening now (radio uses this)
				osThreadFlagsSet(radioServiceTaskID, EVENT_USER_RX_MODE);
				break;
			}

			break;
		case UI_ENTRY_STATE:

			// print user input to the LCD screen
			HD44780_Clear();
			HD44780_PrintStr(messageBuffer);
			HD44780_SetCursor(messageBufferPosition, 0);

			// check for ESC
			checkForReturnToMenu(&key);

			// SEND
			if (key == ENTER_KEY) {
				// validation checks
				if (messageBufferPosition > 0 &&
				        messageBufferPosition <= RADIO_MSG_MAX_SIZE &&
				        messageBufferPosition <= INPUT_BUFFER_SIZE) {

						// build radio message
				        radioMessage_t pendingRadioMessage;
				        pendingRadioMessage.length = messageBufferPosition;
				        memcpy(pendingRadioMessage.data, messageBuffer, strlen(messageBuffer) + 1);

				        osMessageQueuePut(radioSendQueueHandle, &pendingRadioMessage, 0, 0);

				        resetMessageBuffer();
				        HD44780_Clear();
				        HD44780_SetCursor(0,0);
				    } else {
				        APP_LOG(TS_OFF, VLEVEL_M, "[Interaction Service] Invalid input length or empty buffer\n\r");
				    }
				break;
			}

			// remove characters from input buffer
			if (key == BACK_KEY) {
				if (messageBufferPosition != 0) {
					messageBuffer[messageBufferPosition] = 0x0;
					messageBufferPosition--;
					APP_LOG(TS_OFF, VLEVEL_M, "Input Buffer: %s \n\r", messageBuffer);
				}
				break;
			}

			// in entry mode, we build up the buffer
			// valid ASCII
			if (key > 0x19 && key < 0x7F) {

				if (messageBufferPosition < INPUT_BUFFER_SIZE) {
					messageBuffer[messageBufferPosition] = key;
					messageBufferPosition++;
					APP_LOG(TS_OFF, VLEVEL_M, "Input Buffer: %s \n\r", messageBuffer);
				} else {
					// full buffer
					APP_LOG(TS_OFF, VLEVEL_M, "Input buffer full \n\r");
				}
			}

			break;

		case UI_RECEIVE_STATE:

			radioMessage_t incomingMessage;

			// print user receive state to screen
			HD44780_Clear();
			HD44780_SetCursor(0,0);
			HD44780_PrintStr(messageBuffer);

			if (osMessageQueueGet(radioReceiveQueueHandle, &incomingMessage, NULL, 0) == osOK) {
				APP_LOG(TS_OFF, VLEVEL_M, "[Interaction Service] Received message for display: %s \n\r", incomingMessage.data);
				// put message into buffer
				memcpy(messageBuffer, incomingMessage.data, incomingMessage.length);
			}

			// check for ESC key
			checkForReturnToMenu(&key);

			break;
		}

		osDelay(100);  // Poll every 100 ms
	}
}
