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
#include "pid_logic.h"
#include "uart_task.h"
#include "queue.h"

void vTaskGuidance(void *parameters);
extern TIM_HandleTypeDef htim1;
extern QueueHandle_t xUARTQueue;

#endif /* INC_GUIDANCE_TASK_H_ */
