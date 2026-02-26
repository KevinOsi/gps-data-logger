#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "wifi_manager.h"

static const char *TAG = "WIFI_MANAGER";

#define WIFI_SSID      "GPS-Logger-Offload"
#define WIFI_PASS      "password123"
#define WIFI_CHANNEL   1
#define MAX_STA_CONN   4

static bool g_wifi_active = false;
static bool g_wifi_initialized = false;
static esp_netif_t *ap_netif = NULL;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "Station "MACSTR" joined, AID=%d", MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "Station "MACSTR" left, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

esp_err_t wifi_manager_init_softap(void) {
    if (g_wifi_active) return ESP_OK;

    ESP_LOGI(TAG, "Starting Wi-Fi SoftAP...");

    // 1. One-time Initialization
    if (!g_wifi_initialized) {
        // NOTE: esp_netif_init() and event loop are now in main.c
        
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));
        ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                            ESP_EVENT_ANY_ID,
                                                            &wifi_event_handler,
                                                            NULL,
                                                            NULL));
        
        // Create the interface ONLY ONCE
        ap_netif = esp_netif_create_default_wifi_ap();
        g_wifi_initialized = true;
    }

    // 2. Configure Access Point
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .channel = WIFI_CHANNEL,
            .password = WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = { .required = false },
        },
    };
    if (strlen(WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    
    // 3. Start
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "SoftAP Started. SSID:%s Password:%s", WIFI_SSID, WIFI_PASS);

    g_wifi_active = true;
    return ESP_OK;
}

esp_err_t wifi_manager_stop(void) {
    if (!g_wifi_active) return ESP_OK;

    ESP_LOGI(TAG, "Stopping Wi-Fi SoftAP...");
    // We stop the hardware but keep the init/netif config for fast resume
    ESP_ERROR_CHECK(esp_wifi_stop());
    
    g_wifi_active = false;
    return ESP_OK;
}

bool wifi_manager_is_active(void) {
    return g_wifi_active;
}
