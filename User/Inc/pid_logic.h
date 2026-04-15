/*
 * pid_logic.h
 *
 *  Created on: Apr 6, 2026
 *      Author: John
 */

#ifndef PID_LOGIC_H_
#define PID_LOGIC_H_

typedef struct {
    float Kp, Ki, Kd;       // 제어 Gain
    float target;           // 목표값 
    float error_sum;        // 적분항 누적값
    float last_error;       // 이전 오차 (미분항 계산용)
    float out_min, out_max; // 출력 제한 (서보모터 PWM 범위)
    float i_limit;          // Anti-Windup 제한값
} PID_TypeDef;

/* PID 제어 파라미터 튜닝 섹션 */
#define GUIDANCE_TARGET_ANGLE     1500    // 목표: 서보 모터의 중간 각도 (1.5ms 펄스 주기)
#define BASE_KP                   4.0f   // 비례 Gain
#define BASE_KI                   0.5f    // 적분 Gain
#define BASE_KD                   0.5f    // 미분 Gain

/* pid_logic.h */
#define PID_OUT_MIN               -1000.0f  // 1500에서 최대 1000만큼 뺄 수 있음
#define PID_OUT_MAX               1000.0f  // 1500에서 최대 1000만큼 더할 수 있음
#define PID_I_LIMIT               200     // 적분 제어 누적 한계

/* 거리 기반 게인 조절(Gain Scheduling) 설정 */
#define DIST_REF_MM               100.0f  // 기준 거리 (30cm일 때 Kp = BASE_KP)
#define DIST_MAX_FACTOR           2.5f    // 게인이 최대 몇 배까지 커질지 제한
#define DIST_MIN_FACTOR           0.5f    // 게인이 최소 몇 배까지 작아질지 제한


float PID_Compute(PID_TypeDef *pid, float current_angle);

#endif /* PID_LOGIC_H_ */
