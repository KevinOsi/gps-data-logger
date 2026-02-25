#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mock_esp_idf.h"
#include "telemetry.h"
#include "logger_task.h"

// Mock implementations
SemaphoreHandle_t xSemaphoreCreateMutex() { return (void*)1; }
bool xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xBlockTime) { return pdTRUE; }
bool xSemaphoreGive(SemaphoreHandle_t xSemaphore) { return pdTRUE; }
void vTaskDelay(TickType_t xTicksToDelay) {}
uint32_t xPortGetCoreID() { return 1; }
TickType_t xTaskGetTickCount() { return 0; }

QueueHandle_t xQueueCreate(uint32_t uxQueueLength, uint32_t uxItemSize) { return (void*)2; }
bool xQueueSend(QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait) { return pdTRUE; }
bool xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait) { return pdTRUE; }

bool xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode, const char * const pcName, uint32_t usStackDepth, void *pvParameters, uint32_t uxPriority, TaskHandle_t *pvCreatedTask, uint32_t xCoreID) {
    return pdTRUE;
}

int fsync(int fd) { return 0; }

void test_logger_enqueue() {
    printf("Testing Logger Enqueue...\n");
    
    // Initialize task (creates queue)
    logger_task_start();
    
    global_telemetry_t snapshot = {0};
    esp_err_t err = logger_enqueue(&snapshot);
    assert(err == ESP_OK);
    
    printf("Logger Enqueue Test Passed!\n");
}

int main() {
    // Create dummy mount point for local testing
    system("mkdir -p /tmp/sd");
    
    test_logger_enqueue();
    printf("All Logger tests passed!\n");
    return 0;
}
