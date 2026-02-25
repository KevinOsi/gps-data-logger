#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "mock_esp_idf.h"
#include "../main/button_handler.h"

// Simple Queue implementation for testing
typedef struct {
    void* data;
    size_t item_size;
    size_t capacity;
    size_t head;
    size_t tail;
    size_t count;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} mock_queue_t;

QueueHandle_t xQueueCreate(uint32_t uxQueueLength, uint32_t uxItemSize) {
    mock_queue_t* q = malloc(sizeof(mock_queue_t));
    q->data = malloc(uxQueueLength * uxItemSize);
    q->item_size = uxItemSize;
    q->capacity = uxQueueLength;
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    return (QueueHandle_t)q;
}

bool xQueueSend(QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait) {
    mock_queue_t* q = (mock_queue_t*)xQueue;
    pthread_mutex_lock(&q->mutex);
    if (q->count == q->capacity) {
        pthread_mutex_unlock(&q->mutex);
        return false;
    }
    memcpy((char*)q->data + (q->tail * q->item_size), pvItemToQueue, q->item_size);
    q->tail = (q->tail + 1) % q->capacity;
    q->count++;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    return true;
}

bool xQueueSendFromISR(QueueHandle_t xQueue, const void * pvItemToQueue, int *pxHigherPriorityTaskWoken) {
    return xQueueSend(xQueue, pvItemToQueue, 0);
}

bool xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait) {
    mock_queue_t* q = (mock_queue_t*)xQueue;
    pthread_mutex_lock(&q->mutex);
    
    if (q->count == 0) {
        if (xTicksToWait == 0) {
            pthread_mutex_unlock(&q->mutex);
            return false;
        }
        
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        if (xTicksToWait == portMAX_DELAY) {
            while (q->count == 0) {
                pthread_cond_wait(&q->cond, &q->mutex);
            }
        } else {
            long ms = xTicksToWait;
            ts.tv_sec += ms / 1000;
            ts.tv_nsec += (ms % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000) {
                ts.tv_sec++;
                ts.tv_nsec -= 1000000000;
            }
            if (pthread_cond_timedwait(&q->cond, &q->mutex, &ts) != 0) {
                pthread_mutex_unlock(&q->mutex);
                return false;
            }
        }
    }
    
    if (q->count == 0) {
        pthread_mutex_unlock(&q->mutex);
        return false;
    }

    memcpy(pvBuffer, (char*)q->data + (q->head * q->item_size), q->item_size);
    q->head = (q->head + 1) % q->capacity;
    q->count--;
    pthread_mutex_unlock(&q->mutex);
    return true;
}

// GPIO Mocks
esp_err_t gpio_config(const gpio_config_t *pGPIOConfig) { return ESP_OK; }
esp_err_t gpio_install_isr_service(int intr_alloc_flags) { return ESP_OK; }
static void (*g_isr_handler)(void*) = NULL;
static void* g_isr_args = NULL;

esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, void (*isr_handler)(void*), void* args) {
    g_isr_handler = isr_handler;
    g_isr_args = args;
    return ESP_OK;
}

// Timer Mock
static int64_t g_current_time_ms = 0;
int64_t esp_timer_get_time() { return g_current_time_ms * 1000; }

// FreeRTOS Mocks
bool xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode, const char * const pcName, uint32_t usStackDepth, void *pvParameters, uint32_t uxPriority, TaskHandle_t *pvCreatedTask, uint32_t xCoreID) {
    pthread_t thread;
    pthread_create(&thread, NULL, (void* (*)(void*))pvTaskCode, pvParameters);
    return true;
}

int main() {
    printf("Testing Button Handler...\n");
    
    assert(button_handler_init(GPIO_NUM_0) == ESP_OK);
    
    button_event_t event;
    
    // Test Single Press
    printf("Simulating Single Press...\n");
    g_current_time_ms = 1000;
    g_isr_handler(g_isr_args);
    
    // Wait for event (should take ~400ms in the task)
    assert(button_handler_get_event(&event, 1000) == ESP_OK);
    assert(event == BUTTON_EVENT_SINGLE_PRESS);
    printf("Single Press OK\n");
    
    // Test Double Press
    printf("Simulating Double Press...\n");
    g_current_time_ms = 2000;
    g_isr_handler(g_isr_args); // First press
    usleep(10000); // Give task time to wake up
    
    g_current_time_ms = 2100;
    g_isr_handler(g_isr_args); // Second press
    
    assert(button_handler_get_event(&event, 1000) == ESP_OK);
    assert(event == BUTTON_EVENT_DOUBLE_PRESS);
    printf("Double Press OK\n");

    // Test Debounce
    printf("Simulating Debounced Press...\n");
    g_current_time_ms = 3000;
    g_isr_handler(g_isr_args); // Valid press
    g_current_time_ms = 3020;
    g_isr_handler(g_isr_args); // Debounced press (within 50ms)
    
    assert(button_handler_get_event(&event, 1000) == ESP_OK);
    assert(event == BUTTON_EVENT_SINGLE_PRESS); // Should only result in one single press
    printf("Debounce OK\n");
    
    printf("Button Handler tests passed!\n");
    return 0;
}
