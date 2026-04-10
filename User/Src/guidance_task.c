/*
 * guidance_task.c
 *
 *  Created on: 2026. 4. 3.
 *      Author: johnh
 */

#include "guidance_task.h"


void vTaskGuidance(void *parameters)
{
	// 주기 설정
	TickType_t xLastWakeTime = xTaskGetTickCount();
	const TickType_t xFrequency = pdMS_TO_TICKS(20); // 20ms 주기 (50Hz)
	static uint8_t uart_count = 0;
	static uint8_t hit_count = 0;
	static uint8_t search_timeout = 0;
	static uint16_t last_valid_distance = 0;
	static float filtered_dist = 0;
	float alpha = 0.1f;
	float output = 0;
	float final_pwm = 0;

	// 데이터 구조 선언
	uint8_t received_data[4];
	PID_TypeDef hpid;
	UART_Data_t huart;
	ModeType_def mode;

	// PID parameter 설정
	hpid.Kp = BASE_KP;
	hpid.Ki = BASE_KI;
	hpid.Kd = BASE_KD;
	hpid.target = GUIDANCE_TARGET_ANGLE;    // 타겟이 정중앙(90도)에 오도록 유도
	hpid.error_sum = 0;
	hpid.last_error = 0;
	hpid.out_min = PID_OUT_MIN;     // 서보 최소 PWM (예: 0도)
	hpid.out_max = PID_OUT_MAX;    // 서보 최대 PWM (예: 180도)
	hpid.i_limit = PID_I_LIMIT;


	for(;;)
	{
		if (xQueueReceive(xGuidanceQueue, received_data, 0) == pdPASS)
		{
			// 데이터 파싱
			uint16_t distance = (received_data[0] << 8) | received_data[1];
			uint16_t angle = (received_data[2] << 8) | received_data[3];

			// 센서 데이터 필터링
			if (distance == 0 || distance > 2000)
			{
				search_timeout++;
				// 10번 연속(200ms)으로 0이 들어오면 진짜 타겟이 없는 것으로 간주
				if (search_timeout > 3)
				{
					distance = 999; // SEARCH 모드를 유도하기 위한 큰 값
				} else
				{
					distance = last_valid_distance; // 잠시 튀는 거면 이전 값 유지
				}
			} else
			{
				search_timeout = 0;
				last_valid_distance = distance;
			}

			// 범위 밖일떄 ( SEARCH-MODE )
			if ( distance > 300 )
			{
				huart.distance = distance;
				huart.current_angle = 0;
				huart.pid_output = 0;
				mode = SEARCH;
				hit_count = 0;
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 1500);
			}
			else //범위 안일때 LOCKON 혹은 HIT
			{
				if (filtered_dist == 0) filtered_dist = (float)distance; // 초기값 설정

				// 필터 공식: 이전 값 90% + 새 값 10% 섞기
				filtered_dist = (alpha * (float)distance) + ((1.0f - alpha) * filtered_dist);

				float safe_distance = (filtered_dist < 80) ? 80.0f : (float)filtered_dist;
				float distance_factor = DIST_REF_MM / safe_distance;
				hpid.Kp = BASE_KP * distance_factor;

				output = PID_Compute(&hpid, (float)angle);
				final_pwm = 1500.0f + output;
				if (final_pwm > 2500) final_pwm = 2500;
				if (final_pwm < 500)  final_pwm = 500;
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, final_pwm);
				// hit 검사
				// 유효한 데이터가 들어온 경우에만 판단
				if (distance < 60)
				{
					hit_count++;
				} else
				{
					hit_count = 0; // 80mm보다 확실히 멀어졌을 때만 리셋
				}

				if (hit_count >= 5)	mode = HIT;
				else	mode = LOCKON;
			}
			uart_count++;
			// 4. uart Task(100ms 마다 uart_task 실행)
			if (uart_count >= 5 )
			{
				// LCD 구조체에 데이터 입력
				huart.distance = distance;
				huart.current_angle = angle;
				huart.pid_output = (mode == SEARCH) ? 0 : final_pwm;
				huart.mode = mode;

				xQueueSend(xUARTQueue, &huart, pdMS_TO_TICKS(0));
				uart_count = 0;
			}

		}
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

