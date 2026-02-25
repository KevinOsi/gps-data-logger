#include <stdio.h>
#include <assert.h>
#include "mock_esp_idf.h"
#include "sd_card_handler.h"

// Mock implementations
SemaphoreHandle_t xSemaphoreCreateMutex() { return (void*)1; }
bool xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xBlockTime) { return pdTRUE; }
bool xSemaphoreGive(SemaphoreHandle_t xSemaphore) { return pdTRUE; }
void vTaskDelay(TickType_t xTicksToDelay) {}

static sdmmc_card_t mock_card = { .cid = { .name = "MOCK_SD" } };

esp_err_t spi_bus_initialize(int host_id, const spi_bus_config_t *bus_config, int dma_chan) {
    return ESP_OK;
}

esp_err_t esp_vfs_fat_sdspi_mount(const char* base_path, const sdmmc_host_t* host_config,
                                  const sdspi_device_config_t* slot_config,
                                  const esp_vfs_fat_sdmmc_mount_config_t* mount_config,
                                  sdmmc_card_t** out_card) {
    *out_card = &mock_card;
    return ESP_OK;
}

esp_err_t esp_vfs_fat_sdcard_unmount(const char* base_path, sdmmc_card_t* card) {
    return ESP_OK;
}

const char* esp_err_to_name(esp_err_t code) {
    return "MOCK_ERR";
}

void test_sd_card_init_mount() {
    printf("Testing SD Card Init and Mount...\n");
    
    esp_err_t err = sd_card_mount();
    assert(err == ESP_OK);
    
    printf("SD Card Mount Test Passed!\n");
}

int main() {
    test_sd_card_init_mount();
    printf("All SD Card tests passed!\n");
    return 0;
}
