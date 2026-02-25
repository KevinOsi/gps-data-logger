#ifndef MOCK_ESP_IDF_H
#define MOCK_ESP_IDF_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

// Mocking FreeRTOS
#define pdTRUE 1
#define pdFALSE 0
#define portTICK_PERIOD_MS 1
#define portMAX_DELAY (TickType_t)0xffffffff
#define pdMS_TO_TICKS(x) ((x) / portTICK_PERIOD_MS)
typedef void* SemaphoreHandle_t;
typedef void* TaskHandle_t;
typedef uint32_t TickType_t;
typedef void* QueueHandle_t;

SemaphoreHandle_t xSemaphoreCreateMutex();
bool xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xBlockTime);
bool xSemaphoreGive(SemaphoreHandle_t xSemaphore);
uint32_t xPortGetCoreID();
TickType_t xTaskGetTickCount();
void vTaskDelay(TickType_t xTicksToDelay);

QueueHandle_t xQueueCreate(uint32_t uxQueueLength, uint32_t uxItemSize);
bool xQueueSend(QueueHandle_t xQueue, const void * pvItemToQueue, TickType_t xTicksToWait);
bool xQueueSendFromISR(QueueHandle_t xQueue, const void * pvItemToQueue, int *pxHigherPriorityTaskWoken);
bool xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait);

typedef void (*TaskFunction_t)(void *);
bool xTaskCreatePinnedToCore(TaskFunction_t pvTaskCode, const char * const pcName, uint32_t usStackDepth, void *pvParameters, uint32_t uxPriority, TaskHandle_t *pvCreatedTask, uint32_t xCoreID);

// Mocking ESP Drivers
typedef int esp_err_t;
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERR_NOT_FOUND 0x105
#define ESP_ERR_TIMEOUT 0x107

#define IRAM_ATTR

typedef enum {
    GPIO_NUM_0 = 0,
    GPIO_NUM_1,
    GPIO_NUM_4 = 4,
} gpio_num_t;

typedef enum {
    GPIO_INTR_DISABLE = 0,
    GPIO_INTR_POSEDGE,
    GPIO_INTR_NEGEDGE,
    GPIO_INTR_ANYEDGE,
    GPIO_INTR_LOW_LEVEL,
    GPIO_INTR_HIGH_LEVEL,
    GPIO_INTR_MAX,
} gpio_int_type_t;

typedef enum {
    GPIO_MODE_INPUT = 1,
    GPIO_MODE_OUTPUT = 2,
} gpio_mode_t;

typedef struct {
    uint64_t pin_bit_mask;
    gpio_mode_t mode;
    int pull_up_en;
    int pull_down_en;
    gpio_int_type_t intr_type;
} gpio_config_t;

esp_err_t gpio_config(const gpio_config_t *pGPIOConfig);
esp_err_t gpio_install_isr_service(int intr_alloc_flags);
esp_err_t gpio_isr_handler_add(gpio_num_t gpio_num, void (*isr_handler)(void*), void* args);
int gpio_get_level(gpio_num_t gpio_num);

int64_t esp_timer_get_time();

typedef enum {
    UART_NUM_0,
    UART_NUM_1,
    UART_NUM_2,
    UART_NUM_MAX
} uart_port_t;

typedef enum {
    I2C_NUM_0,
    I2C_NUM_MAX
} i2c_port_t;

typedef enum {
    I2C_MODE_MASTER,
    I2C_MODE_MAX
} i2c_mode_t;

esp_err_t i2c_master_write_read_device(i2c_port_t i2c_num, uint8_t device_address,
                                       const uint8_t* write_buffer, size_t write_size,
                                       uint8_t* read_buffer, size_t read_size,
                                       TickType_t ticks_to_wait);

esp_err_t i2c_master_write_to_device(i2c_port_t i2c_num, uint8_t device_address,
                                     const uint8_t* write_buffer, size_t write_size,
                                     TickType_t ticks_to_wait);

int uart_read_bytes(uart_port_t uart_num, void* buf, uint32_t len, TickType_t ticks_to_wait);

// SD Card and SPI Mocks
typedef struct {
    int slot;
} sdmmc_host_t;

typedef struct {
    struct {
        char name[16];
    } cid;
} sdmmc_card_t;

typedef struct {
    bool format_if_mount_failed;
    int max_files;
    int allocation_unit_size;
} esp_vfs_fat_sdmmc_mount_config_t;

typedef struct {
    int mosi_io_num;
    int miso_io_num;
    int sclk_io_num;
    int quadwp_io_num;
    int quadhd_io_num;
    int max_transfer_sz;
} spi_bus_config_t;

typedef struct {
    int gpio_cs;
    int host_id;
} sdspi_device_config_t;

#define SDSPI_HOST_DEFAULT() { .slot = 1 }
#define SDSPI_DEVICE_CONFIG_DEFAULT() { .gpio_cs = -1, .host_id = 0 }
#define SDSPI_DEFAULT_DMA_CHAN 1

esp_err_t spi_bus_initialize(int host_id, const spi_bus_config_t *bus_config, int dma_chan);
esp_err_t esp_vfs_fat_sdspi_mount(const char* base_path, const sdmmc_host_t* host_config,
                                  const sdspi_device_config_t* slot_config,
                                  const esp_vfs_fat_sdmmc_mount_config_t* mount_config,
                                  sdmmc_card_t** out_card);
esp_err_t esp_vfs_fat_sdcard_unmount(const char* base_path, sdmmc_card_t* card);
const char* esp_err_to_name(esp_err_t code);

// Mocking Logging
#define ESP_LOGI(tag, format, ...) printf("LOGI [%s]: " format "\n", tag, ##__VA_ARGS__)
#define ESP_LOGE(tag, format, ...) printf("LOGE [%s]: " format "\n", tag, ##__VA_ARGS__)
#define ESP_LOGD(tag, format, ...) printf("LOGD [%s]: " format "\n", tag, ##__VA_ARGS__)
#define ESP_LOGW(tag, format, ...) printf("LOGW [%s]: " format "\n", tag, ##__VA_ARGS__)

#endif // MOCK_ESP_IDF_H
