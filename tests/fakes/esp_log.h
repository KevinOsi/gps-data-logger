#include <stdio.h>
#include "../mock_esp_idf.h"
#define ESP_LOGI(tag, format, ...) printf("LOGI [%s]: " format "\n", tag, ##__VA_ARGS__)
#define ESP_LOGE(tag, format, ...) printf("LOGE [%s]: " format "\n", tag, ##__VA_ARGS__)
#define ESP_LOGD(tag, format, ...) printf("LOGD [%s]: " format "\n", tag, ##__VA_ARGS__)
