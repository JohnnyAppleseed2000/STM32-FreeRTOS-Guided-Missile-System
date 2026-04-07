/*
 * can_handler.c
 *
 *  Created on: 2026. 4. 3.
 *      Author: johnh
 */

#include "can_handler.h"


HAL_StatusTypeDef CAN1_Tx(uint16_t curr_angle, uint16_t distance, CAN_HandleTypeDef *hcan)
{
	CAN_TxHeaderTypeDef TxHeader;
	uint32_t TxMailbox;
	uint8_t TxData[4];

	TxHeader.DLC = 4;
	TxHeader.StdId = CAN_ID_TARGET_DATA;
	TxHeader.IDE   = CAN_ID_STD;
	TxHeader.RTR = CAN_RTR_DATA;

	// 상위 8 비트
	TxData[0] = (distance >> BIT_POS) & BIT_CONST;
	// 하위 8 비트
	TxData[1] = distance & BIT_CONST;

	TxData[2] = (curr_angle >> BIT_POS) & (BIT_CONST);
	TxData[3] = curr_angle & BIT_CONST;

	return HAL_CAN_AddTxMessage(hcan,&TxHeader,TxData,&TxMailbox);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef RxHeader;
	uint8_t RxData[4];
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
	{
		xQueueSendFromISR(xGuidanceQueue, &RxData, &xHigherPriorityTaskWoken);
	}
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}









