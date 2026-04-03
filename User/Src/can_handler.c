/*
 * can_handler.c
 *
 *  Created on: 2026. 4. 3.
 *      Author: johnh
 */

#include "can_handler.h"
HAL_StatusTypeDef CAN1_Tx(uint16_t curr_angle, uint16_t distance)
{
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;
	uint8_t TxData[4];

	TxHeader.DLC = 4;
	TxHeader.StdId = CAN_ID_TARGET_DATA;
	TxHeader.IDE   = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;

	TxData[0] = distance & BIT_CONST;
	TxData[1] = distance & (BIT_CONST << 4);
	TxData[2] = curr_angle & (BIT_CONST);
	TxData[3] = curr_angle & (BIT_CONST << 4);

	return HAL_CAN_AddTxMessage(&hcan,&TxHeader,TxData,&TxMailbox);
}

