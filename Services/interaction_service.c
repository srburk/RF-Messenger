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
static char inputBuffer[INPUT_BUFFER_SIZE];
static uint8_t inputBufferPosition = 0;

static const char menuTopLeftText[] = "SEND";
static const char menuTopRightText[] = "RECEIVE";
static const char menuBottomText[] = "<- SELECTION ->";

//static const char entryText[] = "ENTER:";

//static const char receivedText[] = "RECEIVED:";

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

static void return_to_menu(uint8_t key) {
	if (key == ESC_KEY) {
		resetInputState();
		HD44780_Clear();
		HD44780_SetCursor(0,0);
		currentUIState = UI_MENU_STATE;
		resetInputState();
	}
}

/* Functions ------------------------------------------------------------------*/

void interactionServiceTask(void *argument) {

	// initialize input buffer
	resetInputState();

	// initialize the LCD screen
	HD44780_Init(4);
	HD44780_Blink();
	HD44780_Clear();

	while (1) {
		read_keyboard(&key);

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
				resetInputState();
				currentUIState = UI_ENTRY_STATE;
			}
			else if (key == RIGHT_ARROW) {
				HD44780_Clear();
				resetInputState();
				currentUIState = UI_RECEIVE_STATE;
			}

			break;
		case UI_ENTRY_STATE:

			// print user input to the LCD screen
			HD44780_Clear();
			HD44780_PrintStr(inputBuffer);
			HD44780_SetCursor(inputBufferPosition, 0);

			// check for ESC
			return_to_menu(key);

			// SEND
			if (key == ENTER_KEY) {
				// validation checks
				if (inputBufferPosition > 0 &&
				        inputBufferPosition <= RADIO_MSG_MAX_SIZE &&
				        inputBufferPosition <= INPUT_BUFFER_SIZE) {

						// build radio message
				        radioMessage_t pendingRadioMessage;
				        pendingRadioMessage.length = inputBufferPosition;
				        memcpy(pendingRadioMessage.data, inputBuffer, strlen(inputBuffer) + 1);

				        osMessageQueuePut(radioInputQueueHandle, &pendingRadioMessage, 0, 0);

				        resetInputState();
				        HD44780_Clear();
				        HD44780_SetCursor(0,0);
				    } else {
				        APP_LOG(TS_OFF, VLEVEL_M, "[Interaction Service] Invalid input length or empty buffer\n\r");
				    }
				break;
			}

			// remove characters from input buffer
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
		case UI_RECEIVE_STATE:

			// print user receive state to screen
			HD44780_Clear();
			HD44780_SetCursor(0,0);
			// Edit :HD44780_PrintStr(Text Here);

			// check for ESC key
			return_to_menu(key);

			break;
		}

		osDelay(100);  // Poll every 100 ms
	}
}
