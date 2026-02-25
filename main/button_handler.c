#include "button_handler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "BUTTON_HANDLER";

#define DEBOUNCE_MS 50
#define DOUBLE_PRESS_MS 400
#define LONG_PRESS_MS 2000

static QueueHandle_t button_event_queue = NULL;
static QueueHandle_t gpio_evt_queue = NULL;

typedef struct {
    uint32_t gpio_num;
    int level;
    uint32_t timestamp;
} gpio_evt_t;

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    gpio_evt_t evt = {
        .gpio_num = gpio_num,
        .level = gpio_get_level(gpio_num),
        .timestamp = (uint32_t)(esp_timer_get_time() / 1000)
    };
    xQueueSendFromISR(gpio_evt_queue, &evt, NULL);
}

static void button_task(void* arg) {
    gpio_evt_t evt;
    button_event_t event;
    uint32_t press_start_time = 0;
    bool is_pressed = false;
    bool waiting_for_double = false;

    while (1) {
        TickType_t wait_ticks = is_pressed ? pdMS_TO_TICKS(100) : (waiting_for_double ? pdMS_TO_TICKS(DOUBLE_PRESS_MS) : portMAX_DELAY);
        
        if (xQueueReceive(gpio_evt_queue, &evt, wait_ticks)) {
            if (evt.level == 0) { // Pressed (Active Low)
                if (!is_pressed) {
                    press_start_time = evt.timestamp;
                    is_pressed = true;
                }
            } else { // Released
                if (is_pressed) {
                    uint32_t press_duration = evt.timestamp - press_start_time;
                    is_pressed = false;

                    if (press_duration < DEBOUNCE_MS) {
                        // Ignore bounce
                        continue;
                    }

                    if (press_duration >= LONG_PRESS_MS) {
                        event = BUTTON_EVENT_LONG_PRESS;
                        xQueueSend(button_event_queue, &event, 0);
                        waiting_for_double = false;
                    } else {
                        if (waiting_for_double) {
                            event = BUTTON_EVENT_DOUBLE_PRESS;
                            xQueueSend(button_event_queue, &event, 0);
                            waiting_for_double = false;
                        } else {
                            waiting_for_double = true;
                        }
                    }
                }
            }
        } else {
            // Timeout reached
            if (is_pressed) {
                // Check for long press while still held
                uint32_t now = (uint32_t)(esp_timer_get_time() / 1000);
                if (now - press_start_time >= LONG_PRESS_MS) {
                    event = BUTTON_EVENT_LONG_PRESS;
                    xQueueSend(button_event_queue, &event, 0);
                    // To prevent multiple long press events, we'll wait for release
                    while (xQueueReceive(gpio_evt_queue, &evt, portMAX_DELAY)) {
                        if (evt.level == 1) break;
                    }
                    is_pressed = false;
                    waiting_for_double = false;
                }
            } else if (waiting_for_double) {
                event = BUTTON_EVENT_SINGLE_PRESS;
                xQueueSend(button_event_queue, &event, 0);
                waiting_for_double = false;
            }
        }
    }
}

esp_err_t button_handler_init(gpio_num_t gpio_num) {
    gpio_evt_queue = xQueueCreate(10, sizeof(gpio_evt_t));
    button_event_queue = xQueueCreate(10, sizeof(button_event_t));

    if (gpio_evt_queue == NULL || button_event_queue == NULL) {
        return ESP_FAIL;
    }

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << gpio_num),
        .pull_down_en = 0,
        .pull_up_en = 1,
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, gpio_isr_handler, (void*) gpio_num);

    xTaskCreatePinnedToCore(button_task, "button_task", 2048, NULL, 10, NULL, 1);

    ESP_LOGI(TAG, "Button handler initialized on GPIO %d (Long Press supported)", gpio_num);
    return ESP_OK;
}

esp_err_t button_handler_get_event(button_event_t *event, TickType_t wait_ticks) {
    if (xQueueReceive(button_event_queue, event, wait_ticks)) {
        return ESP_OK;
    }
    return ESP_ERR_TIMEOUT;
}
