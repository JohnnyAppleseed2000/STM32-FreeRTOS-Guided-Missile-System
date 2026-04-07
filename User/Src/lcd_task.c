/*
 * lcd_task.c
 *
 *  Created on: 2026. 4. 4.
 *      Author: johnh
 */

void vTaskLCD(void *parameters)
{
	LCD_Data_t displayData;
	char buffer[20];
	for (;;)
	{

		// 1. 큐에서 최신 데이터를 기다림 (약 100ms)
		if (xQueueReceive(xLCDQueue, &displayData, pdMS_TO_TICKS(100)) == pdPASS) {
		// 2. 화면 청소 및 데이터 출력
		sprintf(buffer, "Dist: %4d mm", displayData.distance);
		LCD_Goto(0, 0);
		LCD_Puts(buffer);

		sprintf(buffer, "Ang : %4d deg", displayData.current_angle);
		LCD_Goto(0, 1);
		LCD_Puts(buffer);
		}

		// LCD 통신은 느리므로 다른 태스크에게 CPU를 확실히 양보합니다.
		vTaskDelay(pdMS_TO_TICKS(10));
	    }
	}
}

void LCD_Goto(uint8_t x, uint8_t y)
{
	uint8_t address;
	if ( y == 0 )
	{
		address = 0x80 + x;
	}else
	{
		address = 0xC0 + x;
	}
}

void LCD_Print()
{

}
