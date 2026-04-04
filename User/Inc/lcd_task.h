/*
 * lcd_task.h
 *
 *  Created on: 2026. 4. 4.
 *      Author: johnh
 */

#ifndef INC_LCD_TASK_H_
#define INC_LCD_TASK_H_

#include "main.h"
#include "tasks.c"
#include "queue.h"

typedef struct {
    uint16_t distance;
    uint16_t current_angle;
    uint16_t pid_output;
    char system_status[10]; // "FLYING", "HIT", "IDLE" 등
} LCD_Data_t;

extern QueueHandle_t xLCDQueue;


#endif /* INC_LCD_TASK_H_ */
