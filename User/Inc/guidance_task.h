/*
 * guidance_task.h
 *
 *  Created on: 2026. 4. 3.
 *      Author: johnh
 */

#ifndef INC_GUIDANCE_TASK_H_
#define INC_GUIDANCE_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "can_handler.h"

typedef struct {
    uint16_t Kp, Ki, Kd;       // 제어 이득
    uint16_t target;           // 목표값 (예: 90도)
    uint16_t error_sum;        // 적분항 누적값
    uint16_t last_error;       // 이전 오차 (미분항 계산용)
    uint16_t out_min, out_max; // 출력 제한 (서보모터 PWM 범위)
    uint16_t i_limit;          // Anti-Windup 제한값
} PID_TypeDef;

void vTaskGuidance(void *parameters);
extern void PID(uint16_t distance, uint16_t angle);

#endif /* INC_GUIDANCE_TASK_H_ */
