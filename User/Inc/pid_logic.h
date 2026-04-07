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

/* PID 제어 파라미터 튜닝 섹션 */
#define GUIDANCE_TARGET_ANGLE     1500    // 목표: 기체 정중앙 (PWM 펄스 기준 1.5ms)
#define BASE_KP                   15.0f   // 기본 비례 게인 (실험 시작점)
#define BASE_KI                   0.5f    // 기본 적분 게인 (작게 시작)
#define BASE_KD                   5.0f    // 기본 미분 게인 (진동 억제용)

#define PID_OUT_MIN               500    // 날개 서보 최소 PWM (1ms)
#define PID_OUT_MAX               2500    // 날개 서보 최대 PWM (2ms)
#define PID_I_LIMIT               200     // I-Term 누적 한계 (Anti-Windup)

/* 거리 기반 게인 조절(Gain Scheduling) 설정 */
#define DIST_REF_MM               50.0f  // 기준 거리 (30cm일 때 Kp = BASE_KP)
#define DIST_MAX_FACTOR           2.5f    // 게인이 최대 몇 배까지 커질지 제한
#define DIST_MIN_FACTOR           0.5f    // 게인이 최소 몇 배까지 작아질지 제한


uint16_t PID_Compute(PID_TypeDef *pid, uint16_t current_angle);

#endif /* PID_LOGIC_H_ */
