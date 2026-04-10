#  Guided Missile System Simulator (STM32 & FreeRTOS)

> **STM32F103RB** 환경에서 **FreeRTOS**를 활용하여 구현한 초음파 센서 기반 유도 제어 시스템입니다.

---

## 기술 스택
- **HW:** STM32F103RB (ARM Cortex-M3), HC-SR04(센서 모듈), 서브 모터 2개(SG-90), CAN 트랜시버
- **OS:** FreeRTOS(Task Management, Queue, Notify)
- **통신:** CAN
- **사용 언어:** C

## 설계 구조
- **태스크 분류**
  - (1) Task_sensor (Priority 2) : *SEARCH-MODE* 에서 센서를 단 서브모터를 (45도 - 135도)로 회전시키며 물체를 탐지한다. 물체가 탐지되면 CAN 통신을 통해 현재 각도와 물체와의 거리 데이터 전송.
  <br>
  - (2) Task_guidance (Priority 3) : CAN Interrupt에서 Queue를 통해 데이터를 수신. 물체가 감지되면 *LOCK-ON* 모드로 변경하고 PID 함수를 통해 날개를 제어한다. 물체가 일정 거리안으로 들어오면 *HIT*로 상태변경. 물체가 감지 영역을 벗어나면 다시 *SEARCH-MODE* 로 전환.
  <br>
  - (3) Task_uart (Priority 1) : 현재 상태, 물체를 바라보는 각도, 거리, 그리고 PID 결과를 출력한다.
  <br>

- **Real-time Task Scheduling:** FreeRTOS를 이용한 센서 읽기, 제어 연산, 모터 출력 태스크 분리
- **PID Control:** 타겟과의 거리 및 각도 유지를 위한 정밀 제어 알고리즘 적용. 
## 데모 영상
- **좌상단: 센서**
- **좌하단: 날개**
- **우하단: 결과 출력**
  https://github.com/user-attachments/assets/616ef99a-52f3-412c-bb51-823e04e3723b

## 제어 로직
본 프로젝트에서는 타겟 추적을 위해 아래와 같은 **PID 제어식**을 사용하였습니다.

$$u(t) = K_p e(t) + K_i \int_{0}^{t} e(\tau) d\tau + K_d \frac{de(t)}{dt}$$

<img width="1201" height="399" alt="echo trigger 주기" src="https://github.com/user-attachments/assets/be5e07d0-8b89-44bc-9a1b-4cc463a47f07" />

<img width="1528" height="315" alt="can 통신" src="https://github.com/user-attachments/assets/2211b5f9-8dbc-4b0d-a878-384419e9b7e1" />

