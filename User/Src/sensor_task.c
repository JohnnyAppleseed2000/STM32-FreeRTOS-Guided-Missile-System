/*
 * sensor_task.c
 *
 *  Created on: 2026. 4. 2.
 *      Author: johnh
 */

#include "sensor_task.h"

static void SensorTrigger(void);
static void us_delay(uint8_t);
static void ServoMotorUpdate(uint16_t*, uint16_t*);
static SystemMode_t TrackTarget(uint16_t*, uint16_t*);
static uint16_t distance_calculator(uint32_t);
static SystemMode_t QuickSearch(uint16_t *angle, uint16_t *dist);


void vTaskSensorScanner(void* parameters)
{
	uint16_t current_angle = START_ANGLE;
	uint16_t angle_step = ANGLE_STEP;
	BaseType_t status;
	uint32_t ulNotificationValue;
	SystemMode_t current_mode = MODE_SEARCH;

	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);
	HAL_TIM_IC_Start(&htim4, TIM_CHANNEL_1);

	for(;;)
	{
		// 1. 정찰모드
		if (current_mode == MODE_SEARCH)
		{
			// 센서 트리거
			SensorTrigger();
			// input capture 대기
			status = xTaskNotifyWait(0, 0, &ulNotificationValue, pdMS_TO_TICKS(30));
			//test
			if (status == pdTRUE)
			{
				uint16_t distance = distance_calculator(ulNotificationValue);
				if (distance == 0) continue;
				if (distance <= SENSE_MAX_DISTANCE && distance > SENSE_MIN_DISTANCE)
				{
					current_mode = MODE_LOCKON;	//표적 감지 (LOCK-ON)
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, current_angle);
					continue;
				}
				else
				{
					if (CAN1_Tx(current_angle, distance, &hcan) != HAL_OK)
					{
						Error_Handler();
					}
				}
			}
			// 서브모터 위치 제어
			ServoMotorUpdate(&current_angle, &angle_step);
			vTaskDelay(pdMS_TO_TICKS(60));
		}
		else if( current_mode == MODE_LOCKON )
		{
			HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
			uint16_t final_dist = 0;
			current_mode = TrackTarget(&current_angle, &final_dist);
			if (current_mode == MODE_LOCKON )
			{
				if (CAN1_Tx(current_angle, final_dist, &hcan) != HAL_OK)
				{
					Error_Handler();
				}
			}
		}
	}
}

static void ServoMotorUpdate(uint16_t *angle, uint16_t *step)
{
	*angle += *step;
	if (*angle >= MAX_PULSE || *angle <= MIN_PULSE)
	{
		*step *= -1;	//끝에 도달하면 방향전환
	}
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, *angle);
}

static SystemMode_t TrackTarget(uint16_t *angle, uint16_t *dist)
{
	uint32_t ulTrackValue;
	static uint8_t initial_wait = 0; // LOCKON 진입 직후인지 확인
	static uint8_t missing_count = 0;

	// 처음 LOCKON 진입했을 때는 모터가 멈출 시간을 충분히 준다
	if (initial_wait == 0) {
		vTaskDelay(pdMS_TO_TICKS(80)); // 모터 진동이 멈출 때까지 대기
		initial_wait = 1;
	}

	while(xTaskNotifyWait(0, 0, NULL, 0) == pdTRUE);

	// 1. 표적이 감지되면 현재 각도 유지
	SensorTrigger();
	if ( xTaskNotifyWait(0 , 0, &ulTrackValue, pdMS_TO_TICKS(30)))
	{

		uint16_t d = distance_calculator(ulTrackValue);

		if (d==0)	return MODE_LOCKON;

		if (d <= SENSE_MAX_DISTANCE && d > SENSE_MIN_DISTANCE)
		{
			missing_count = 0;
			*dist = d;

			return MODE_LOCKON;	// 유효거리안에 있으면 계속 LOCKON 유지
		}
	}
	missing_count++;
	// 표적이 15번 감지되지않을때
	if (missing_count < 5) {
		return MODE_LOCKON; // 아직은 제자리에서 더 기다려봄
	} else {
		missing_count = 0;
		initial_wait = 0; // 다시 탐색 할 준비
		return QuickSearch(angle, dist);
	}
}

static SystemMode_t QuickSearch(uint16_t *angle, uint16_t *dist)
{
	uint8_t offset = 50;
	uint32_t ulSearchValue;
	uint16_t base_angle = *angle;
	uint16_t temp_dist;

	//총 3번 감지
	for (int i = 0; i < 5; i++)
	{
		//오른쪽
		uint16_t r_angle = (base_angle + offset > MAX_PULSE) ? MAX_PULSE : base_angle + offset;
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, r_angle);
		vTaskDelay(pdMS_TO_TICKS(30));
		SensorTrigger();
		if (xTaskNotifyWait(0, 0, &ulSearchValue, pdMS_TO_TICKS(30)) == pdTRUE)
		{
			temp_dist = distance_calculator(ulSearchValue);
			// 유효거리 안
			if (temp_dist <= SENSE_MAX_DISTANCE && temp_dist > SENSE_MIN_DISTANCE)
			{
				*angle = r_angle;
				*dist = temp_dist;
				return MODE_LOCKON;
			}
		}

		//왼쪽
		uint16_t l_angle = (base_angle < MIN_PULSE + offset) ? MIN_PULSE : base_angle - offset;
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, l_angle);
		vTaskDelay(pdMS_TO_TICKS(30));
		SensorTrigger();
		if ( xTaskNotifyWait(0, 0, &ulSearchValue, pdMS_TO_TICKS(30)) == pdTRUE)
		{
			temp_dist = distance_calculator(ulSearchValue);
			if (temp_dist <= SENSE_MAX_DISTANCE && temp_dist > SENSE_MIN_DISTANCE)
			{
				*angle = l_angle;
				*dist = temp_dist;
				return MODE_LOCKON;
			}
		}
		offset += 20;
	}
	// 3번 빠른 탐색 이후 감지 못했으면 MODE_SERCH 반환
	return MODE_SEARCH;

}

static void SensorTrigger(void)
{
	HAL_GPIO_WritePin(Trigger_GPIO_Port, Trigger_Pin, GPIO_PIN_SET);
	us_delay(10);
	HAL_GPIO_WritePin(Trigger_GPIO_Port, Trigger_Pin, GPIO_PIN_RESET);

}

static void us_delay(uint8_t us)
{
	// 현재 타이머 숫자 읽음
	uint32_t start_tick = DWT->CYCCNT;
	// 1us에 64ticks
	uint32_t target_ticks = us * (SystemCoreClock / 1000000);
	while ((DWT->CYCCNT - start_tick) < target_ticks);
}

static uint16_t distance_calculator(uint32_t notified_value)
{
	// 거리 (mm)
	uint16_t distance = notified_value * 10 / US_TO_DISTANCE;

	return distance;
}


// ISR : 타이머 캡쳐
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    // TIM4 인지 확인
    if (htim->Instance == TIM4)
    {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
            uint32_t capture_val = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
            // xHigherPriorityTaskWoken = pdFALSE로 초기화
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            // 현재 실행중인 태스크의 우선순위가 더 낮다면 xHigherPriorityTaskWoken = pdTRUE로 전환
            xTaskNotifyFromISR(xSensorScannerHandle,
                               capture_val,
                               eSetValueWithOverwrite,
                               &xHigherPriorityTaskWoken);

            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }
}
