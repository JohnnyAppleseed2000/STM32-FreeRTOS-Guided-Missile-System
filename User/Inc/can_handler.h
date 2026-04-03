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


#define BIT_CONST	0xF
#define CAN_ID_TARGET_DATA	0x101D;

extern CAN_HandleTypeDef hcan;

HAL_StatusTypeDef CAN1_Tx(uint16_t, uint16_t);
//HAL_StatusTypeDef CAN1_Rx(uint16_t, uint16_t);

#endif /* INC_CAN_HANDLER_H_ */
