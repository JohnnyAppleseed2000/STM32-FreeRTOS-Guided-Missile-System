/*
 * sensor_task.h
 *
 *  Created on: 2026. 4. 2.
 *      Author: johnh
 */

#ifndef TASKS_INC_SENSOR_TASK_H_
#define TASKS_INC_SENSOR_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "can_handler.h"

extern TaskHandle_t xSensorScannerHandle;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

// distance calculating constant
#define US_TO_DISTANCE	58U

// servo-motor macros
#define START_ANGLE		1500U
#define	ANGLE_STEP		50U
#define MAX_PULSE		2500U
#define MIN_PULSE		500U

// sensor macros
#define SENSE_MAX_DISTANCE	100U
#define SENSE_MIN_DISTANCE	2U

// quick search macro
#define MAX_SEARCH_NUM 		3

typedef enum
{
	MODE_SEARCH,
	MODE_LOCKON
}SystemMode_t;

void vTaskSensorScanner(void* parameters);






#endif /* TASKS_INC_SENSOR_TASK_H_ */
