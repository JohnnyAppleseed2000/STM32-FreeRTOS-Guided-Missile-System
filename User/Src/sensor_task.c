/*
 * sensor_task.c
 *
 *  Created on: 2026. 4. 2.
 *      Author: johnh
 */

#include "sensor_task.h"

static void SensorTrigger(void);
static void us_delay(uint8_t);
static void ServoMotorUpdate(uint16_t*, uint8_t*);
static SystemMode_t TrackTarget(uint16_t*, uint16_t*);
static uint16_t distance_calculator(uint32_t);


void vTaskSensorScanner(void* parameters)
{
	uint16_t current_angle = START_ANGLE;
	uint8_t angle_step = ANGLE_STEP;
	BaseType_t status;
	uint32_t ulNotificationValue;
	SystemMode_t current_mode = MODE_SEARCH;

	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2);

	for(;;)
	{
		// 1. 정찰모드
		if (current_mode == MODE_SEARCH)
		{
			// 서브모터 위치 제어
			ServoMotorUpdate(&current_angle, &angle_step);

			// 센서 트리거
			SensorTrigger();

			// input capture 대기
			status = xTaskNotifyWait(0, 0, &ulNotificationValue, pdMS_TO_TICKS(10));
			// 거리 계산
			if (status == pdTRUE)
			{
				uint16_t distance = distance_calculator(ulNotificationValue);
				if (distance <= SENSE_MAX_DISTANCE && distance > SENSE_MIN_DISTANCE)
				{
					current_mode = MODE_LOCKON;	//표적 감지 (LOCK-ON)
				}
			}
			// CPU 양보 및 측정 주기 확보 (매뉴얼 권장 60ms)
			vTaskDelay(pdMS_TO_TICKS(60));
		}else if( current_mode == MODE_LOCKON )
		{
			uint16_t final_dist = 0;
			current_mode = TrackTarget(&current_angle, &final_dist);
			if (current_mode == MODE_LOCKON )
			{
				if (CAN1_Tx(current_angle, final_dist) != HAL_OK)
				{
					Error_Handler();
				}
				vTaskDelay(pdMS_TO_TICKS(40));
			}
		}
	}
}

static void ServoMotorUpdate(uint16_t *angle, uint8_t *step)
{
	*angle += *step;
	if (*angle >= MAX_PULSE || *angle <= MIN_PULSE)
	{
		*step *= -1;	//끝에 도달하면 방향전환
	}
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, *angle);
	vTaskDelay(pdMS_TO_TICKS(60));
}

static SystemMode_t TrackTarget(uint16_t *angle, uint16_t *dist)
{
	uint8_t track_step = 50;
	BaseType_t rstatus, lstatus;
	uint32_t ulRTrackValue, ulLTrackValue;
	uint16_t r_dist, l_dist;

	//오른쪽
	uint16_t r_angle = *angle + track_step;
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, r_angle);
	vTaskDelay(pdMS_TO_TICKS(60));
	SensorTrigger();
	rstatus = xTaskNotifyWait(0, 0, &ulRTrackValue, pdMS_TO_TICKS(10));
	if(rstatus == pdTRUE)	r_dist = distance_calculator(ulRTrackValue);

	//왼쪽
	uint16_t l_angle = *angle - track_step;
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, l_angle);
	vTaskDelay(pdMS_TO_TICKS(60));
	SensorTrigger();
	lstatus = xTaskNotifyWait(0, 0, &ulLTrackValue, pdMS_TO_TICKS(10));
	if(lstatus == pdTRUE)	l_dist = distance_calculator(ulLTrackValue);

	if (r_dist < SENSE_MAX_DISTANCE || l_dist < SENSE_MAX_DISTANCE)
	{
		*angle = (r_dist <= l_dist) ? r_angle : l_angle;
		*dist = (r_dist <= l_dist) ? r_dist : l_dist;
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, *angle);
		return MODE_LOCKON;
	}
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
