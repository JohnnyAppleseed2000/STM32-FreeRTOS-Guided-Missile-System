/*
 * pid_logic.c
 *
 *  Created on: 2026. 4. 4.
 *      Author: johnh
 */

#include "main.h"
#include "pid_logic.h"

// PID 계산 함수
uint16_t PID_Compute(PID_TypeDef *pid, uint16_t current_angle)
{
    uint16_t error = pid->target - current_angle;

    // P 항
    uint16_t p_out = pid->Kp * error;

    // I 항 (Anti-Windup 적용)
    pid->error_sum += error;
    if (pid->error_sum > pid->i_limit) pid->error_sum = pid->i_limit;
    else if (pid->error_sum < -pid->i_limit) pid->error_sum = -pid->i_limit;
    uint16_t i_out = pid->Ki * pid->error_sum;

    // D 항
    uint16_t d_out = pid->Kd * (error - pid->last_error);
    pid->last_error = error;

    uint16_t output = p_out + i_out + d_out;

    // 출력 제한 (Saturation)
    if (output > pid->out_max) output = pid->out_max;
    else if (output < pid->out_min) output = pid->out_min;

    return output;
}
