#ifndef UI_TASK_H
#define UI_TASK_H

/**
 * @brief FreeRTOS task function for UI rendering on OLED.
 * 
 * @param pvParameters Task parameters (unused).
 */
void ui_task(void *pvParameters);

/**
 * @brief Initialize and start the UI task on Core 1.
 */
void ui_task_start();

#endif // UI_TASK_H
