#include "sd_card_handler.h"
#include "hw_config.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdspi_host.h"
#include "driver/spi_common.h"
#include "esp_log.h"
#include "ff.h"

static const char *TAG = "SD_CARD_HANDLER";
static sdmmc_card_t *card = NULL;

#define MOUNT_POINT "/sd"

esp_err_t sd_card_get_info(uint32_t *out_total_mb, uint32_t *out_free_mb) {
    if (card == NULL) return ESP_FAIL;

    FATFS *fs;
    DWORD fre_clust, fre_sect, tot_sect;

    /* Get volume information and free clusters of drive 0 */
    FRESULT res = f_getfree("0:", &fre_clust, &fs);
    if (res != FR_OK) {
        ESP_LOGE(TAG, "Failed to get SD card info (res=%d)", res);
        return ESP_FAIL;
    }

    /* Get total sectors and free sectors */
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;

    /* Convert to MB */
    // Sector size is usually 512 bytes
    *out_total_mb = (uint32_t)(tot_sect / (2048)); // (tot_sect * 512) / (1024 * 1024)
    *out_free_mb = (uint32_t)(fre_sect / (2048));

    return ESP_OK;
}

esp_err_t sd_card_mount(void) {
    esp_err_t ret;

    // 1. Configure Mount Options
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    ESP_LOGD(TAG, "Initializing SD card on VSPI (SPI3)...");

    // 2. Physical "Warm-up" - SD cards need ~80 clock cycles with CS high to enter SPI mode
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SD_CS_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 1,
    };
    gpio_config(&io_conf);
    gpio_set_level(SD_CS_PIN, 1); // CS High
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // 3. Initialize SPI Bus
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = SPI3_HOST; 
    host.max_freq_khz = 400; // Initialization frequency must be low

    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SD_MOSI_PIN,
        .miso_io_num = SD_MISO_PIN,
        .sclk_io_num = SD_SCK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4000,
    };
    
    // Attempt to initialize bus
    ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) { // Ignore if already init
        ESP_LOGE(TAG, "Failed to initialize SPI bus.");
        return ret;
    }

    // 4. Attach SD card to the SPI bus
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SD_CS_PIN;
    slot_config.host_id = host.slot;

    ESP_LOGD(TAG, "Mounting filesystem...");
    // We try multiple times as SD cards are temperamental
    int retry = 3;
    while (retry--) {
        ret = esp_vfs_fat_sdspi_mount(MOUNT_POINT, &host, &slot_config, &mount_config, &card);
        if (ret == ESP_OK) break;
        ESP_LOGW(TAG, "Mount attempt failed (0x%x), retrying...", ret);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize the card (0x%x). Check wiring/pull-ups.", ret);
        return ret;
    }

    ESP_LOGI(TAG, "SD Card mounted: %s", card->cid.name);
    return ESP_OK;
}

void sd_card_unmount(void) {
    if (card) {
        esp_vfs_fat_sdcard_unmount(MOUNT_POINT, card);
        card = NULL;
        ESP_LOGI(TAG, "SD Card unmounted");
    }
}
