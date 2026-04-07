/*
 * can_handler.h
 *
 *  Created on: 2026. 4. 3.
 *      Author: johnh
 */

#ifndef INC_CAN_HANDLER_H_
#define INC_CAN_HANDLER_H_

#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"

#define BIT_POS		8
#define BIT_CONST	0xFF
#define CAN_ID_TARGET_DATA	0x101;

extern CAN_HandleTypeDef hcan;
extern QueueHandle_t xGuidanceQueue;

HAL_StatusTypeDef CAN1_Tx(uint16_t, uint16_t, CAN_HandleTypeDef*);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef*);

#endif /* INC_CAN_HANDLER_H_ */
