#ifndef LOGGER_TASK_H
#define LOGGER_TASK_H

#include "esp_err.h"
#include "telemetry.h"

/**
 * @brief Start the logger task on Core 1.
 */
void logger_task_start(void);

/**
 * @brief Enqueue a telemetry snapshot for logging.
 * 
 * @param snapshot Pointer to the telemetry snapshot.
 * @return esp_err_t ESP_OK if enqueued successfully.
 */
esp_err_t logger_enqueue(const global_telemetry_t *snapshot);

#endif // LOGGER_TASK_H
