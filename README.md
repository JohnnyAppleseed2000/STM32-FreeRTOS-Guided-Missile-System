#  Guided Missile System Simulator (STM32 & FreeRTOS)

> **STM32F103RB** 환경에서 **FreeRTOS**를 활용하여 구현한 초음파 센서를 활용한 PID 날개 제어 시스템입니다.

---

## 기술 스택
- **HW:** STM32F103RB (ARM Cortex-M3), HC-SR04(센서 모듈), 서브 모터 2개(SG-90), CAN 트랜시버, 로직 에널라이저(디버깅용)
- **SW:** STMCubeIDE. STMCubeMX, Putty, PulseView
- **OS:** FreeRTOS(Task Management, Queue, Notify)
- **통신:** CAN
- **사용 언어:** C

## 설계 구조
- **태스크 분류**
  - (1) Task_sensor (Priority 2) : *SEARCH-MODE* 에서 초음파 센서를 단 서브모터를 (45도 - 135도)로 회전시키며 물체를 탐지한다. 물체가 탐지되면 *LOCK-ON* 모드로 변경 후 CAN 통신을 통해 현재 모터의 각도와 물체와의 거리 데이터 전송.
  &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;**↓** **↑**
  - (2) Task_guidance (Priority 3) : CAN Interrupt에서 Queue를 통해 데이터를 수신.  *LOCK-ON* 모드면 PID 함수를 통해 날개를 제어한다. 물체가 일정 거리안으로 들어오면 *HIT*로 상태변경. 물체가 감지 영역을 벗어나면 다시 *SEARCH-MODE* 로 전환.
  &emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;**↓**
  - (3) Task_uart (Priority 1) : 현재 상태, 물체를 바라보는 각도, 거리, 그리고 PID 결과를 출력한다.
  <br>

- **Real-time Task Scheduling:** FreeRTOS를 이용한 센서 읽기, 제어 연산, 모터 출력 태스크 분리
- **PID Control:** 타겟과의 거리 및 각도 유지를 위한 정밀 제어 알고리즘 적용. 
## 데모 영상
- **좌상단: 센서**
- **좌하단: 날개**
- **우하단: 결과 출력**

<video src="https://github.com/user-attachments/assets/616ef99a-52f3-412c-bb51-823e04e3723b" controls="controls" style="max-width: 100%;"></video>

## 로직 에널라이저를 통한 CAN 통신 출력
-**CAN 통신**

<img width="1018" height="210" alt="can 통신" src="https://github.com/user-attachments/assets/2211b5f9-8dbc-4b0d-a878-384419e9b7e1" />



## 제어 로직
본 프로젝트에서는 타겟 추적을 위해 아래와 같은 **PID 제어식**을 사용했다.

$$u(t) = K_p e(t) + K_i \int_{0}^{t} e(\tau) d\tau + K_d \frac{de(t)}{dt}$$



#### 1. 가변 게인 PID 제어 
타겟과의 거리에 따라 제어 강도를 동적으로 조절하는 **Gain Scheduling** 기법을 적용했습니다.

* **배경:** 타겟이 멀리 있을 때는 완만하게 추적하고, 가까워질수록 제어 반응성을 높여 명중률을 향상시켜야 합니다.
* **구현:** 타겟과의 거리($d$)에 반비례하도록 $K_p$ 게인을 실시간으로 업데이트합니다.
    $$K_p(d) = K_{p,base} \times \frac{d_{ref}}{d_{current}}$$

---

#### 2. 신호 처리 및 필터링 (Signal Processing)
저가형 초음파 센서의 하드웨어적 한계를 소프트웨어적으로 극복했습니다.

##### 지수 이동 평균 필터 (EMA Filter)
센서의 미세한 떨림과 노이즈를 제거하여 서보 모터의 지터(Jitter) 현상을 방지합니다.
* **공식:** $y[n] = \alpha \cdot x[n] + (1 - \alpha) \cdot y[n-1]$
* **설정:** $\alpha = 0.3$ (이전 데이터의 가중치를 높게 두어 급격한 변화 억제)

##### 예외 처리 (Search Timeout)
센서 데이터가 일시적으로 손실(0 또는 2000mm 초과)될 경우, **3회(약 60ms)** 동안은 이전의 유효한 데이터를 유지하여 제어의 연속성을 보장합니다.

---





