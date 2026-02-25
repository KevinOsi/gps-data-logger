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

static const char *TAG = "WEB_SERVER";

#define BASE_PATH "/sd"
#define FILE_SERVER_MAX_FILE_SIZE "200KB"

static httpd_handle_t server = NULL;

static esp_err_t download_get_handler(httpd_req_t *req) {
    char filepath[1024];
    FILE *fd = NULL;
    struct stat file_stat;

    const char *filename = req->uri;
    if (filename[0] == '/') filename++;

    if (strlen(filename) == 0) {
        // Directory listing
        httpd_resp_sendstr_chunk(req, "<html><body><h1>SD Card Files</h1><ul>");
        DIR *dir = opendir(BASE_PATH);
        struct dirent *ent;
        if (dir) {
            while ((ent = readdir(dir)) != NULL) {
                if (ent->d_type == DT_REG) {
                    char link[1024];
                    snprintf(link, sizeof(link), "<li><a href=\"/%s\">%s</a></li>", ent->d_name, ent->d_name);
                    httpd_resp_sendstr_chunk(req, link);
                }
            }
            closedir(dir);
        }
        httpd_resp_sendstr_chunk(req, "</ul></body></html>");
        httpd_resp_sendstr_chunk(req, NULL);
        return ESP_OK;
    }

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
