#include "app_api.h"


static const char* TAG = "APP_API";

static httpd_handle_t app_api_httpd_handle = NULL;
static httpd_config_t app_api_httpd_config;


static esp_err_t api_test(httpd_req_t* req)
{
    app_gpio_test();
    char* resp = "Test complete";
    httpd_resp_send(req, resp, strlen(resp));
    return ESP_OK;
}

static esp_err_t api_restart(httpd_req_t* req)
{
    char* resp = "Restarting...";
    httpd_resp_send(req, resp, strlen(resp));
    esp_restart();
    return ESP_OK;
}

static esp_err_t api_factoryreset(httpd_req_t* req)
{
    ESP_ERROR_CHECK(nvs_flash_erase());
    ESP_ERROR_CHECK(nvs_flash_init());
    char* resp = "All settings were erased. Default will be used.";
    httpd_resp_send(req, resp, strlen(resp));
    return ESP_OK;
}


void app_api_common_register_endpoints(httpd_handle_t httpd)
{
    httpd_uri_t test = {
        .uri = "/api/test",
        .method = HTTP_POST,
        .handler = api_test,
    };
    httpd_register_uri_handler(httpd, &test);

    httpd_uri_t restart = {
        .uri = "/api/restart",
        .method = HTTP_POST,
        .handler = api_restart,
    };
    httpd_register_uri_handler(httpd, &restart);

    httpd_uri_t factoryreset = {
        .uri = "/api/factoryreset",
        .method = HTTP_POST,
        .handler = api_factoryreset,
    };
    httpd_register_uri_handler(httpd, &factoryreset);
}


void app_api_setup()
{
    httpd_config_t def = HTTPD_DEFAULT_CONFIG();
    memcpy(&app_api_httpd_config, &def, sizeof(httpd_config_t));

    app_api_httpd_config.max_uri_handlers = 32;
}

bool app_api_up()
{
    ESP_LOGI(TAG, "Starting server on port: '%d'", app_api_httpd_config.server_port);

    esp_err_t err = httpd_start(&app_api_httpd_handle, &app_api_httpd_config);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "Error starting server!");
        ESP_ERROR_CHECK(err);
        return false;
    }

    ESP_LOGI(TAG, "Registering endpoints");
    app_api_common_register_endpoints(app_api_httpd_handle);
    app_api_wifi_register_endpoints(app_api_httpd_handle);
    app_api_mqtt_register_endpoints(app_api_httpd_handle);
    app_api_switch_register_endpoints(app_api_httpd_handle);

    return true;
}

esp_err_t app_api_httpd_read_json(httpd_req_t* req, char** out_content, cJSON** out_json)
{
    *out_content = malloc(req->content_len);
    if (httpd_req_recv(req, *out_content, req->content_len) <= 0) {
        free(*out_content);
        return ESP_FAIL;
    }
    *out_json = cJSON_Parse(*out_content);
    if (*out_json == NULL) {
        ESP_LOGE(TAG, "JSON error at: %s\n", cJSON_GetErrorPtr());
        cJSON_Delete(*out_json);
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t app_api_httpd_send_json(httpd_req_t* req, cJSON* json)
{
    char* json_str = cJSON_Print(json);
    return httpd_resp_send(req, json_str, strlen(json_str));
}
