/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.c
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "sys_app.h"
#include "subghz_phy_app.h"
#include "radio.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */
#include "global_config.h"
#include "application_service.h"

#include "stm32_timer.h"
#include "sys_app.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum
{
  RADIO_IDLE,
  RADIO_RX,
  RADIO_TX,
} radioState_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/*Timeout*/
#define RX_TIMEOUT_VALUE              3000
#define TX_TIMEOUT_VALUE              3000

/* Afc bandwidth in Hz */
#define FSK_AFC_BANDWIDTH             83333

#define MAX_APP_BUFFER_SIZE          255
#if (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE)
#error PAYLOAD_LEN must be less or equal than MAX_APP_BUFFER_SIZE
#endif /* (PAYLOAD_LEN > MAX_APP_BUFFER_SIZE) */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */
osMessageQueueId_t radioInputQueueHandle = NULL;

static uint8_t BufferRx[MAX_APP_BUFFER_SIZE];
static radioState_t radioState = RADIO_RX;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief Function to be executed on Radio Tx Done event
 */
static void OnTxDone(void);

/**
  * @brief Function to be executed on Radio Rx Done event
  * @param  payload ptr of buffer received
  * @param  size buffer size
  * @param  rssi
  * @param  LoraSnr_FskCfo
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
  * @brief Function executed on Radio Tx Timeout event
  */
static void OnTxTimeout(void);

/**
  * @brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/**
  * @brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/* USER CODE BEGIN PFP */
static void RadioISRCallback(void);
/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */

  /* USER CODE END SubghzApp_Init_1 */

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

  /* USER CODE BEGIN SubghzApp_Init_2 */

  APP_LOG(TS_OFF, VLEVEL_M, "---------------\n\r");
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_MODULATION\n\r");
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_BW=%d Hz\n\r", FSK_BANDWIDTH);
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_DR=%d bits/s\n\r", FSK_DATARATE);
  APP_LOG(TS_OFF, VLEVEL_M, "TX POWER=%i dbm\n\r", TX_OUTPUT_POWER);

  Radio.SetChannel(RF_FREQUENCY);

  Radio.SetTxConfig(MODEM_FSK, TX_OUTPUT_POWER, FSK_FDEV, 0,
					FSK_DATARATE, 0,
					FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
					true, 0, 0, 0, TX_TIMEOUT_VALUE);

  Radio.SetRxConfig(MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE,
					0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
					0, FSK_FIX_LENGTH_PAYLOAD_ON, 0, true,
					0, 0, false, true);

  Radio.SetMaxPayloadLength(MODEM_FSK, MAX_APP_BUFFER_SIZE);

  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
  APP_LOG(TS_ON, VLEVEL_L, "OnTxDone\n\r");
  /* USER CODE END OnTxDone */
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */
  APP_LOG(TS_ON, VLEVEL_L, "OnRxDone\n\r");
  APP_LOG(TS_ON, VLEVEL_L, "RssiValue=%d dBm, Cfo=%dkHz\n\r", rssi, LoraSnr_FskCfo);

  if (size == 0 || size > APP_BUFFER_SIZE) {
	  APP_LOG(TS_ON, VLEVEL_L, "Invalid RX size: %d\n\r", size);
	  return;
  }

//  if (memchr(data, '\0', size) == NULL) {
//	  APP_LOG(TS_ON, VLEVEL_L, "Invalid string on RX \n\r");
//	  return;
//  }

  radioMessage_t receivedMessage = {0};

  memcpy(receivedMessage.data, payload, size);
  receivedMessage.length = size;

  APP_LOG(TS_ON, VLEVEL_L, "Received string: %s\n\r", receivedMessage.data);

  RadioISRCallback();

  /* USER CODE END OnRxDone */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
	APP_LOG(TS_ON, VLEVEL_L, "TX Timeout \n\r");
  /* USER CODE END OnTxTimeout */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */
  APP_LOG(TS_ON, VLEVEL_L, "RX Timeout \n\r");
  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
  APP_LOG(TS_ON, VLEVEL_L, "RX Error \n\r");
  /* USER CODE END OnRxError */
}

/* USER CODE BEGIN PrFD */

// callback to be triggered to let application service know
static void RadioISRCallback(void) {
	uint32_t flags = 0x00000001U; // Define the flag to set
	osThreadFlagsSet(applicationServiceTaskID, flags);
}

void radioServiceTask(void *argument) {

	// init radio service

	radioMessage_t incomingMessage;

    for (;;) {

    	// could be refactored for background listening later

    	switch (radioState) {
    	case RADIO_IDLE:

			Radio.Sleep();
			osDelay(2);

    		if (osMessageQueueGet(radioInputQueueHandle, &incomingMessage, NULL, 0) == osOK) {
		        APP_LOG(TS_OFF, VLEVEL_M, "[Radio Service] Sending message: %s \n\r", incomingMessage.data);
				Radio.Send(incomingMessage.data, incomingMessage.length);
			}

    		break;

    	case RADIO_RX:

    		// continuous listening
    		Radio.Rx(0);

    		break;

    	case RADIO_TX:
    		break;
    	}

        osDelay(100);

//    	Radio.Sleep()

//    	if (osMessageQueueGet(radioInputQueueHandle, &incomingMessage, NULL, osWaitForever) == osOK) {
//			// Send the data
////    		APP_LOG(TS_ON, VLEVEL_M, "Calling Radio.Send for message: %s \n\r", incomingMessage.data);
//    		Radio.Send(incomingMessage.data, PAYLOAD_LEN);
//		}

    }
}
/* USER CODE END PrFD */
