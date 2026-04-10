/*
 * pid_logic.c
 *
 *  Created on: 2026. 4. 4.
 *      Author: johnh
 */

#include "main.h"
#include "pid_logic.h"

// PID 계산 함수
// pid_logic.c 수정 버전
float PID_Compute(PID_TypeDef *pid, float current_angle)
{
    // 1. 타입을 float 또는 int16_t로 변경하여 음수 오차 허용
    float error = (float)pid->target - current_angle;
    float dt = 0.02f; // 20ms 주기를 초 단위로 환산

    // P 항
    float p_out = pid->Kp * error;

    // I 항
    pid->error_sum += (error * dt);

    // Anti-Windup
    if (pid->error_sum > pid->i_limit) pid->error_sum = pid->i_limit;
    else if (pid->error_sum < -pid->i_limit) pid->error_sum = -pid->i_limit;

    float i_out = pid->Ki * pid->error_sum;

    // D 항
    float d_out = pid->Kd * (error - pid->last_error) / dt;
    pid->last_error = error;

    // 전체 출력 계산
    float total_output = p_out + i_out + d_out;

    // 여기서 -500 ~ 500 사이로 가둡니다.
	if (total_output > pid->out_max) total_output = pid->out_max;
	else if (total_output < pid->out_min) total_output = pid->out_min;

    return total_output;
}
