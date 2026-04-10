/*
 * uart_task.h
 *
 *  Created on: Apr 9, 2026
 *      Author: John
 */

#ifndef INC_UART_TASK_H_
#define INC_UART_TASK_H_

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

typedef	enum
{
	LOCKON,
	HIT,
	SEARCH
}ModeType_def;

typedef struct {
    uint16_t distance;
    uint16_t current_angle;
    uint16_t pid_output;
    ModeType_def mode; // "LOCKDOWN", "HIT"
} UART_Data_t;

//macros
#define _ADDR	(0x27 << 1)

void vTaskUART(void *parameters);

extern QueueHandle_t xUARTQueue;

#endif /* INC_UART_TASK_H_ */
