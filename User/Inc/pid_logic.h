/*
 * pid_logic.h
 *
 *  Created on: Apr 6, 2026
 *      Author: John
 */

#ifndef PID_LOGIC_H_
#define PID_LOGIC_H_

typedef struct {
    uint16_t Kp, Ki, Kd;       // 제어 이득
    uint16_t target;           // 목표값 (예: 90도)
    uint16_t error_sum;        // 적분항 누적값
    uint16_t last_error;       // 이전 오차 (미분항 계산용)
    uint16_t out_min, out_max; // 출력 제한 (서보모터 PWM 범위)
    uint16_t i_limit;          // Anti-Windup 제한값
} PID_TypeDef;

uint16_t PID_Compute(PID_TypeDef *pid, uint16_t current_angle);

#endif /* PID_LOGIC_H_ */
