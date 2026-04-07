/*
 * guidance_task.c
 *
 *  Created on: 2026. 4. 3.
 *      Author: johnh
 */

#include "guidance_task.h"


void vTaskGuidance(void *parameters)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(20); // 20ms 주기 (50Hz)
	uint8_t received_data[4];
	PID_TypeDef hpid;

	// 1. PID 파라미터 초기화 (튜닝이 필요합니다!)
	hpid.Kp = 1;  hpid.Ki = 0.01; hpid.Kd = 1;
	hpid.target = 90;    // 타겟이 정중앙(90도)에 오도록 유도
	hpid.error_sum = 0;     hpid.last_error = 0;
	hpid.out_min = 500;     // 서보 최소 PWM (예: 0도)
	hpid.out_max = 2500;    // 서보 최대 PWM (예: 180도)
	hpid.i_limit = 50;
	uint16_t base_Kp = 1;
	for(;;)
	{
		if (xQueueReceive(xGuidanceQueue, received_data, portMAX_DELAY) == pdPASS)
		{
			uint16_t distance = (received_data[0] << 8) | received_data[1];
			uint16_t angle = (received_data[2] << 8) | received_data[3];

			uint16_t distance_factor = 100 / distance; //10cm
			if (distance_factor > 2) distance_factor = 2;
			hpid.Kp = base_Kp * distance_factor;
			// TO-DO 여기서 100ms 마다 lcd task 불러주기
			// 2. 터미널 로직 (종말 유도)
			uint16_t output = PID_Compute(&hpid, angle);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, output);
		}
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

