#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "ble_manager.h"

static const char *TAG = "BLE_MANAGER";

static uint8_t ble_addr_type;
static bool g_is_connected = false;
static uint16_t g_val_handle_temp;
static uint16_t g_val_handle_press;
static uint16_t g_val_handle_hum;
static uint16_t g_val_handle_lat;
static uint16_t g_val_handle_lon;
static uint16_t g_val_handle_alt;
static uint16_t g_val_handle_heading;

// Custom UUIDs for GPS data
// DECAFBAD-1111-2222-3333-444455556666
static const ble_uuid128_t g_svc_uuid_gps =
    BLE_UUID128_INIT(0x66, 0x66, 0x55, 0x55, 0x44, 0x44, 0x33, 0x33, 0x22, 0x22, 0x11, 0x11, 0xad, 0xfb, 0xca, 0xde);

static const ble_uuid128_t g_chr_uuid_lat =
    BLE_UUID128_INIT(0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xad, 0xfb, 0xca, 0xde);
static const ble_uuid128_t g_chr_uuid_lon =
    BLE_UUID128_INIT(0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xad, 0xfb, 0xca, 0xde);
static const ble_uuid128_t g_chr_uuid_alt =
    BLE_UUID128_INIT(0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xad, 0xfb, 0xca, 0xde);
static const ble_uuid128_t g_chr_uuid_heading =
    BLE_UUID128_INIT(0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xad, 0xfb, 0xca, 0xde);

static int ble_manager_gatt_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

static const struct ble_gatt_svc_def g_ble_gatt_svcs[] = {
    {
        // Environmental Sensing Service
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x181A),
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = BLE_UUID16_DECLARE(0x2A6E), // Temperature
                .access_cb = ble_manager_gatt_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &g_val_handle_temp,
            },
            {
                .uuid = BLE_UUID16_DECLARE(0x2A6D), // Pressure
                .access_cb = ble_manager_gatt_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &g_val_handle_press,
            },
            {
                .uuid = BLE_UUID16_DECLARE(0x2A6F), // Humidity
                .access_cb = ble_manager_gatt_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &g_val_handle_hum,
            },
            { 0 }
        },
    },
    {
        // GPS/Navigation Service (Custom)
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &g_svc_uuid_gps.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = &g_chr_uuid_lat.u,
                .access_cb = ble_manager_gatt_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &g_val_handle_lat,
            },
            {
                .uuid = &g_chr_uuid_lon.u,
                .access_cb = ble_manager_gatt_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &g_val_handle_lon,
            },
            {
                .uuid = &g_chr_uuid_alt.u,
                .access_cb = ble_manager_gatt_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &g_val_handle_alt,
            },
            {
                .uuid = &g_chr_uuid_heading.u,
                .access_cb = ble_manager_gatt_cb,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &g_val_handle_heading,
            },
            { 0 }
        },
    },
    { 0 }
};

static global_telemetry_t g_latest_telemetry = {0};

static int ble_manager_gatt_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    if (ctxt->op != BLE_GATT_ACCESS_OP_READ_CHR) return BLE_ATT_ERR_UNLIKELY;

    if (attr_handle == g_val_handle_temp) {
        int16_t temp = (int16_t)(g_latest_telemetry.env.temperature * 100);
        return os_mbuf_append(ctxt->om, &temp, sizeof temp) == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == g_val_handle_press) {
        uint32_t press = (uint32_t)(g_latest_telemetry.env.pressure * 10);
        return os_mbuf_append(ctxt->om, &press, sizeof press) == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == g_val_handle_hum) {
        uint16_t hum = (uint16_t)(g_latest_telemetry.env.humidity * 100);
        return os_mbuf_append(ctxt->om, &hum, sizeof hum) == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == g_val_handle_lat) {
        return os_mbuf_append(ctxt->om, &g_latest_telemetry.gps.lat, sizeof(int32_t)) == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == g_val_handle_lon) {
        return os_mbuf_append(ctxt->om, &g_latest_telemetry.gps.lon, sizeof(int32_t)) == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == g_val_handle_alt) {
        float alt = g_latest_telemetry.env.altitude; // Default to barometric altitude
        return os_mbuf_append(ctxt->om, &alt, sizeof(float)) == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    } else if (attr_handle == g_val_handle_heading) {
        float heading = g_latest_telemetry.mag.heading; // Use magnetometer heading
        return os_mbuf_append(ctxt->om, &heading, sizeof(float)) == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

static int ble_manager_gap_event(struct ble_gap_event *event, void *arg);

static void ble_manager_advertise(void) {
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    memset(&fields, 0, sizeof fields);
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name = (uint8_t *)"GPS-Logger";
    fields.name_len = strlen("GPS-Logger");
    fields.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "error setting advertisement data; rc=%d", rc);
        return;
    }

    memset(&adv_params, 0, sizeof adv_params);
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_manager_gap_event, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "error enabling advertisement; rc=%d", rc);
        return;
    }
}

static int ble_manager_gap_event(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI(TAG, "BLE connected; status=%d", event->connect.status);
            if (event->connect.status == 0) {
                g_is_connected = true;
            } else {
                ble_manager_advertise();
            }
            return 0;

        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGI(TAG, "BLE disconnected; reason=%d", event->disconnect.reason);
            g_is_connected = false;
            ble_manager_advertise();
            return 0;

        case BLE_GAP_EVENT_ADV_COMPLETE:
            ESP_LOGI(TAG, "BLE advertise complete; reason=%d", event->adv_complete.reason);
            ble_manager_advertise();
            return 0;

        default:
            return 0;
    }
}

static void ble_manager_on_sync(void) {
    int rc;
    rc = ble_hs_util_ensure_addr(0);
    assert(rc == 0);
    rc = ble_hs_id_infer_auto(0, &ble_addr_type);
    assert(rc == 0);
    ble_manager_advertise();
}

static void ble_manager_host_task(void *param) {
    ESP_LOGI(TAG, "BLE Host Task Started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

esp_err_t ble_manager_init(void) {
    esp_err_t ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init nimble_port");
        return ret;
    }

    ble_hs_cfg.sync_cb = ble_manager_on_sync;
    
    // Initialize standard GATT and GAP services
    ble_svc_gap_init();
    ble_svc_gatt_init();

    int rc = ble_gatts_count_cfg(g_ble_gatt_svcs);
    if (rc != 0) return ESP_FAIL;

    rc = ble_gatts_add_svcs(g_ble_gatt_svcs);
    if (rc != 0) return ESP_FAIL;

    // Set device name
    rc = ble_svc_gap_device_name_set("GPS-Logger");
    if (rc != 0) {
        ESP_LOGE(TAG, "Failed to set device name; rc=%d", rc);
        return ESP_FAIL;
    }

    // Start the host task
    nimble_port_freertos_init(ble_manager_host_task);

    return ESP_OK;
}

esp_err_t ble_manager_update_telemetry(const global_telemetry_t *snapshot) {
    memcpy(&g_latest_telemetry, snapshot, sizeof(global_telemetry_t));
    
    if (g_is_connected) {
        // Notify clients of changes
        ble_gatts_chr_updated(g_val_handle_temp);
        ble_gatts_chr_updated(g_val_handle_press);
        ble_gatts_chr_updated(g_val_handle_hum);
        ble_gatts_chr_updated(g_val_handle_lat);
        ble_gatts_chr_updated(g_val_handle_lon);
        ble_gatts_chr_updated(g_val_handle_alt);
        ble_gatts_chr_updated(g_val_handle_heading);
    }
    return ESP_OK;
}

esp_err_t ble_manager_set_advertising(bool enable) {
    if (enable) {
        ble_manager_advertise();
    } else {
        ble_gap_adv_stop();
    }
    return ESP_OK;
}

bool ble_manager_is_connected(void) {
    return g_is_connected;
}
