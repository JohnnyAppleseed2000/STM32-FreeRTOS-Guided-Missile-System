/*
 * guidance_task.c
 *
 *  Created on: 2026. 4. 3.
 *      Author: johnh
 */

#include "guidance_task.h"

void vTaskGuidance(void *parameters)
{
	uint8_t received_data[4];

	for(;;) {
		if (xQueueReceive(xGuidanceQueue, received_data, portMAX_DELAY) == pdPASS) {
			// 데이터 파싱 및 날개 제어 로직 실행
			uint16_t distance = (received_data[0] << 8) | received_data[1];
			uint16_t angle = (received_data[2] << 8) | received_data[3];
		}
	}

}

static void PID(uint16_t distance, uint16_t angle)
{

}

