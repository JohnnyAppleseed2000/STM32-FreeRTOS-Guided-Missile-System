/*
 * pid_logic.c
 *
 *  Created on: 2026. 4. 4.
 *      Author: johnh
 */


typedef struct {
    uint16_t Kp, Ki, Kd;       // 제어 이득
    uint16_t target;           // 목표값 (예: 90도)
    uint16_t error_sum;        // 적분항 누적값
    uint16_t last_error;       // 이전 오차 (미분항 계산용)
    uint16_t out_min, out_max; // 출력 제한 (서보모터 PWM 범위)
    uint16_t i_limit;          // Anti-Windup 제한값
} PID_TypeDef;

// PID 계산 함수
uint16_t PID_Compute(PID_TypeDef *pid, uint16_t current_angle) {
     error = pid->target - current_val;

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
