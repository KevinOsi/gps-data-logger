#ifndef TELEMETRY_TASK_H
#define TELEMETRY_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/**
 * @brief FreeRTOS task function for telemetry data acquisition.
 * 
 * @param pvParameters Task parameters (unused).
 */
void telemetry_task(void *pvParameters);

/**
 * @brief Initialize and start the telemetry task on Core 0.
 */
void telemetry_task_start();

#endif // TELEMETRY_TASK_H
