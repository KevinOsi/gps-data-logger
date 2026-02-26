#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_vfs.h"
#include "web_server.h"
#include "telemetry.h"

static const char *TAG = "WEB_SERVER";

#define BASE_PATH "/sd"

static httpd_handle_t server = NULL;

const char* HTML_STYLE = 
"<style>"
"body { font-family: sans-serif; margin: 20px; background: #f0f2f5; color: #333; }"
"h1 { color: #1a73e8; }"
".card { background: white; padding: 15px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); margin-bottom: 20px; }"
".grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); gap: 10px; }"
".sensor { border-left: 4px solid #1a73e8; padding-left: 10px; }"
".sensor label { font-size: 0.8em; color: #666; display: block; }"
".sensor span { font-size: 1.2em; font-weight: bold; }"
"table { width: 100%; border-collapse: collapse; background: white; border-radius: 8px; overflow: hidden; }"
"th, td { padding: 12px; text-align: left; border-bottom: 1px solid #eee; }"
"th { background: #1a73e8; color: white; }"
"tr:hover { background: #f8f9fa; }"
"a { color: #1a73e8; text-decoration: none; font-weight: bold; }"
".btn { display: inline-block; padding: 6px 12px; background: #1a73e8; color: white !important; border-radius: 4px; font-size: 0.9em; }"
"</style>";

static esp_err_t download_get_handler(httpd_req_t *req) {
    char filepath[1024];
    FILE *fd = NULL;
    struct stat file_stat;

    const char *filename = req->uri;
    if (filename[0] == '/') filename++;

    // Ignore favicon
    if (strcmp(filename, "favicon.ico") == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Not found");
        return ESP_OK;
    }

    if (strlen(filename) == 0) {
        // --- Directory listing & Dashboard ---
        global_telemetry_t tele;
        if (xSemaphoreTake(g_telemetry_mutex, 50 / portTICK_PERIOD_MS) == pdTRUE) {
            memcpy(&tele, &g_telemetry, sizeof(global_telemetry_t));
            xSemaphoreGive(g_telemetry_mutex);
        }

        httpd_resp_sendstr_chunk(req, "<html><head><title>GPS Logger</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
        httpd_resp_sendstr_chunk(req, HTML_STYLE);
        httpd_resp_sendstr_chunk(req, "</head><body><h1>GPS Data Logger</h1>");

        // Sensor Dashboard
        httpd_resp_sendstr_chunk(req, "<div class=\"card\"><h3>Live Sensors</h3><div class=\"grid\">");
        
        char buf[1024];
        snprintf(buf, sizeof(buf), 
            "<div class=\"sensor\"><label>Position</label><span>%.6f, %.6f</span></div>"
            "<div class=\"sensor\"><label>Altitude%s</label><span>%.1fm</span></div>"
            "<div class=\"sensor\"><label>Heading</label><span>%.1f&deg;</span></div>"
            "<div class=\"sensor\"><label>Temperature</label><span>%.1f&deg;C</span></div>"
            "<div class=\"sensor\"><label>Pressure</label><span>%.1f hPa</span></div>"
            "<div class=\"sensor\"><label>Humidity</label><span>%.1f%%</span></div>",
            tele.gps.lat / 10000000.0, tele.gps.lon / 10000000.0,
            tele.alt_calibrated ? " (Cal)" : " (?)", tele.fused_alt,
            tele.mag.heading,
            tele.env.temperature, tele.env.pressure, tele.env.humidity);
        httpd_resp_sendstr_chunk(req, buf);
        httpd_resp_sendstr_chunk(req, "</div></div>");

        // File List
        httpd_resp_sendstr_chunk(req, "<h3>Recorded Logs</h3><table><thead><tr><th>Filename</th><th>Size</th><th>Action</th></tr></thead><tbody>");
        
        DIR *dir = opendir(BASE_PATH);
        struct dirent *ent;
        if (dir) {
            while ((ent = readdir(dir)) != NULL) {
                if (ent->d_type == DT_REG) {
                    char fpath[1024];
                    snprintf(fpath, sizeof(fpath), "%s/%s", BASE_PATH, ent->d_name);
                    struct stat st;
                    long size = 0;
                    if (stat(fpath, &st) == 0) size = st.st_size;

                    snprintf(buf, sizeof(buf), 
                        "<tr><td>%s</td><td>%ld bytes</td><td><a href=\"/%s\" class=\"btn\">Download</a></td></tr>", 
                        ent->d_name, size, ent->d_name);
                    httpd_resp_sendstr_chunk(req, buf);
                }
            }
            closedir(dir);
        }
        httpd_resp_sendstr_chunk(req, "</tbody></table></body></html>");
        httpd_resp_sendstr_chunk(req, NULL);
        return ESP_OK;
    }

    // --- File Download Logic ---
    snprintf(filepath, sizeof(filepath), "%s/%s", BASE_PATH, filename);

    if (stat(filepath, &file_stat) == -1) {
        ESP_LOGE(TAG, "Failed to stat file : %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File does not exist");
        return ESP_FAIL;
    }

    fd = fopen(filepath, "r");
    if (!fd) {
        ESP_LOGE(TAG, "Failed to read existing file : %s", filepath);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read file");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Sending file : %s (%ld bytes)...", filepath, file_stat.st_size);
    httpd_resp_set_type(req, "application/octet-stream");

    char *chunk = malloc(4096);
    if (!chunk) {
        fclose(fd);
        return ESP_ERR_NO_MEM;
    }
    
    size_t chunksize;
    do {
        chunksize = fread(chunk, 1, 4096, fd);
        if (chunksize > 0) {
            if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK) {
                fclose(fd);
                free(chunk);
                ESP_LOGE(TAG, "File sending failed!");
                return ESP_FAIL;
            }
        }
    } while (chunksize != 0);

    fclose(fd);
    free(chunk);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t download_uri = {
    .uri       = "/*",
    .method    = HTTP_GET,
    .handler   = download_get_handler,
    .user_ctx  = NULL
};

esp_err_t web_server_start(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.stack_size = 8192; 

    ESP_LOGI(TAG, "Starting HTTP Server...");
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &download_uri);
        return ESP_OK;
    }

    ESP_LOGE(TAG, "Error starting server!");
    return ESP_FAIL;
}

esp_err_t web_server_stop(void) {
    if (server) {
        httpd_stop(server);
        server = NULL;
    }
    return ESP_OK;
}
