/*
 * input_service.c
 *
 *  Created on: Apr 4, 2025
 *      Author: sam
 */

#include "i2c.h"
#include "input_service.h"

#include "stm32_timer.h"
#include "sys_app.h"

// Private Define

#define KEYBOARD_I2C_DELAY 20

// Public Variables

osThreadId_t inputServiceTaskID = NULL;

// Private Function Defs

static void read_keyboard(uint8_t *key);

// Private Functions

static void read_keyboard(uint8_t *key) {
    if (HAL_I2C_Master_Receive(&hi2c2, 0x5F << 1, key, 1, KEYBOARD_I2C_DELAY) != HAL_OK) {
        *key = 0;  // No key received
    }
}

// Public Functions

void inputServiceTask(void *argument) {
	while (1) {
		uint8_t key;
		read_keyboard(&key);

		if (key != 0) {
			APP_LOG(TS_OFF, VLEVEL_M, "Key pressed: %c \n\r", key);
		}

		osDelay(100);  // Poll every 100 ms
	}
}
