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

void vTaskGuidance(void *parameters);
extern TIM_HandleTypeDef htim4;

#endif /* INC_GUIDANCE_TASK_H_ */
