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
	// parameter 설정
	hpid.Kp = BASE_KP;
	hpid.Ki = BASE_KI;
	hpid.Kd = BASE_KD;
	hpid.target = GUIDANCE_TARGET_ANGLE;    // 타겟이 정중앙(90도)에 오도록 유도
	hpid.error_sum = 0;
	hpid.last_error = 0;
	hpid.out_min = PID_OUT_MIN;     // 서보 최소 PWM (예: 0도)
	hpid.out_max = PID_OUT_MAX;    // 서보 최대 PWM (예: 180도)
	hpid.i_limit = PID_I_LIMIT;

	uint16_t base_Kp = 1;

	for(;;)
	{
		if (xQueueReceive(xGuidanceQueue, received_data, 0) == pdPASS)
		{
			// 1. 데이터 파싱
			uint16_t distance = (received_data[0] << 8) | received_data[1];
			uint16_t angle = (received_data[2] << 8) | received_data[3];

			// 2. 거리에 따른 각도 제어 배수 설정 (가까우면 더 많이 꺾기)
			float safe_distance = (distance < 10) ? 10.0f : (float)distance;
			float distance_factor = DIST_REF_MM / safe_distance;
			hpid.Kp = base_Kp * distance_factor;

			// 3. PID 계산 후 각도 변경
			uint16_t output = PID_Compute(&hpid, (float)angle);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, output);

			// 4. LCD Task
			if ( distance < )

		}
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

