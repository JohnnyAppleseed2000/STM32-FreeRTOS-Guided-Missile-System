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
uint16_t PID_Compute(PID_TypeDef *pid, float current_angle)
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

    // 서보 모터 기준점(예: 1500)을 더해줘야 할 수도 있음
    // 여기서는 일단 saturation만 처리
    if (total_output > pid->out_max) total_output = pid->out_max;
    else if (total_output < pid->out_min) total_output = pid->out_min;

    return (uint16_t)total_output;
}
