/*
 * uart_task.c
 *
 *  Created on: Apr 9, 2026
 *      Author: John
 */

#include "uart_task.h"
#include "guidance_task.h"
#include <stdio.h>

void vTaskUART(void *parameters)
{
	UART_Data_t displayData;
	for (;;)
	{
		// 1. 큐에서 최신 데이터를 기다림
		if (xQueueReceive(xUARTQueue, &displayData, portMAX_DELAY) == pdPASS)
		{
			// 1. 화면 지우기
			printf("\033[H\033[J");

			if (displayData.mode == HIT || displayData.mode == LOCKON)
			{
				// 2. 대시보드 형태로 출력
				printf("========== MISSILE STATUS ==========\r\n");
				printf(" MODE     : %s\r\n", (displayData.mode == HIT) ? "HIT !!!" : "LOCK-ON");
				printf(" DISTANCE : %d mm\r\n", displayData.distance);
				printf(" ANGLE    : %d\r\n", (displayData.current_angle - 500) * 9 / 100);
				printf(" PID_OUTPUT    : %d\r\n", (displayData.pid_output - 500) * 9/ 100);
				printf("====================================\r\n");
			}else
			{
				printf("========== MISSILE STATUS ==========\r\n");
				printf(" MODE     : %s\r\n", "SEARCHING-MODE");
				printf(" DISTANCE : %d mm\r\n", 0);
				printf(" ANGLE    : %d\r\n", 0);
				printf(" PID_OUTPUT    : %d\r\n", 0);
				printf("====================================\r\n");
			}
		}

	}
}


