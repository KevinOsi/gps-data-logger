#include "button_handler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "BUTTON_HANDLER";

static QueueHandle_t button_event_queue = NULL;
static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void button_task(void* arg) {
    uint32_t io_num;
    button_event_t event;
    uint32_t last_press_time = 0;
    bool waiting_for_double = false;

    while (1) {
        if (xQueueReceive(gpio_evt_queue, &io_num, waiting_for_double ? pdMS_TO_TICKS(400) : portMAX_DELAY)) {
            uint32_t now = esp_timer_get_time() / 1000;
            
            // Debounce: ignore if too close to last press (50ms)
            if (now - last_press_time < 50) {
                continue;
            }
            last_press_time = now;

            if (!waiting_for_double) {
                waiting_for_double = true;
            } else {
                // Double Press detected
                event = BUTTON_EVENT_DOUBLE_PRESS;
                xQueueSend(button_event_queue, &event, 0);
                waiting_for_double = false;
            }
        } else {
            // Timeout reached, Single Press detected
            if (waiting_for_double) {
                event = BUTTON_EVENT_SINGLE_PRESS;
                xQueueSend(button_event_queue, &event, 0);
                waiting_for_double = false;
            }
        }
    }
}

esp_err_t button_handler_init(gpio_num_t gpio_num) {
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    button_event_queue = xQueueCreate(10, sizeof(button_event_t));

    if (gpio_evt_queue == NULL || button_event_queue == NULL) {
        return ESP_FAIL;
    }

    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << gpio_num),
        .pull_down_en = 0,
        .pull_up_en = 1,
    };
    gpio_config(&io_conf);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(gpio_num, gpio_isr_handler, (void*) gpio_num);

    xTaskCreatePinnedToCore(button_task, "button_task", 2048, NULL, 10, NULL, 1);

    ESP_LOGI(TAG, "Button handler initialized on GPIO %d", gpio_num);
    return ESP_OK;
}

esp_err_t button_handler_get_event(button_event_t *event, TickType_t wait_ticks) {
    if (xQueueReceive(button_event_queue, event, wait_ticks)) {
        return ESP_OK;
    }
    return ESP_ERR_TIMEOUT;
}
