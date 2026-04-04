/*
 * guidance_task.c
 *
 *  Created on: 2026. 4. 3.
 *      Author: johnh
 */

#include "guidance_task.h"

typedef struct {
    uint16_t Kp, Ki, Kd;       // 제어 이득
    uint16_t target;           // 목표값 (예: 90도)
    uint16_t error_sum;        // 적분항 누적값
    uint16_t last_error;       // 이전 오차 (미분항 계산용)
    uint16_t out_min, out_max; // 출력 제한 (서보모터 PWM 범위)
    uint16_t i_limit;          // Anti-Windup 제한값
} PID_TypeDef;

void vTaskGuidance(void *parameters)
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(20); // 20ms 주기 (50Hz)
	uint8_t received_data[4];
	PID_Typedef hpid;

	// 1. PID 파라미터 초기화 (튜닝이 필요합니다!)
	hpid.Kp = 1.2U;  hpid.Ki = 0.01U; hpid.Kd = 0.5U;
	hpid.target = 90.0U;    // 타겟이 정중앙(90도)에 오도록 유도
	hpid.error_sum = 0;     hpid.last_error = 0;
	hpid.out_min = 1000U;     // 서보 최소 PWM (예: 0도)
	hpid.out_max = 2000U;    // 서보 최대 PWM (예: 180도)
	hpid.i_limit = 50U;
	uint16_t base_Kp = 1.2;
	for(;;)
	{
		if (xQueueReceive(xGuidanceQueue, received_data, 0) == pdPASS)
		{
			uint16_t distance = (received_data[0] << 8) | received_data[1];
			uint16_t angle = (received_data[2] << 8) | received_data[3];

			uint16_t distance_factor = 100 / distance; //10cm
			if (distance_factor > 2) distance_factor = 2;
			hpid.Kp = base_Kp * distance_factor;
			// TO-DO 여기서 100ms 마다 lcd task 불러주기
			// 2. 터미널 로직 (종말 유도)
			if (distance < 10)
			{ // 10cm 이내 접근 시
				// 목표 달성! 서보를 중앙으로 정렬하고 제어 일시 정지
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 1500);
				// LCD에 "HIT!" 메시지 보낼 준비 (나중에 구현)
				continue;
			}
			uint16_t output = PID_compute(&hpid, angle);
			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, output);
			vTaskDelayUntil(&xLastWakeTime, xFrequency);
		}
	}
}

