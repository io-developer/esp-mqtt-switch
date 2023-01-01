#include "app_api.h"


static cJSON* sta_config_to_json(wifi_sta_config_t* conf)
{
    cJSON* json = cJSON_CreateObject();

    cJSON_AddStringToObject(json, "ssid", (char*)conf->ssid);
    cJSON_AddStringToObject(json, "password", (char*)conf->password);

    return json;
}

static void sta_config_from_json(const cJSON* json, wifi_sta_config_t* dst, const wifi_sta_config_t* src)
{
    memset(dst->ssid, 0, sizeof(dst->ssid));
    sprintf((char*)dst->ssid, "%s", app_util_json_get_str(json, "ssid", (char*)src->ssid));

    memset(dst->password, 0, sizeof(dst->password));
    sprintf((char*)dst->password, "%s", app_util_json_get_str(json, "password", (char*)src->password));
}


static esp_err_t api_wifi_sta_config_get(httpd_req_t *req)
{
    wifi_config_t conf = {};
    ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA, &conf));

    cJSON* json = sta_config_to_json(&conf.sta);
    esp_err_t err = app_api_httpd_send_json(req, json);
    cJSON_Delete(json);
    return err;
}

static esp_err_t api_wifi_sta_config_update(httpd_req_t *req)
{
    char* content = NULL;
    cJSON *json = NULL;
    esp_err_t err = app_api_httpd_read_json(req, &content, &json);
    if (err != ESP_OK) {
        return err;
    }

    wifi_config_t src = {};
    ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA, &src));

    wifi_config_t dst = {};
    memcpy(&dst, &src, sizeof(wifi_config_t));

    ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA, &dst));

    sta_config_from_json(json, &dst.sta, &src.sta);
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &dst));

    cJSON* resp_json = sta_config_to_json(&dst.sta);
    err = app_api_httpd_send_json(req, resp_json);

    cJSON_Delete(resp_json);
    cJSON_Delete(json);

    return err;
}


void app_api_wifi_register_endpoints(httpd_handle_t httpd)
{
    static httpd_uri_t config_get = {
        .uri = "/api/wifi/config",
        .method = HTTP_GET,
        .handler = api_wifi_sta_config_get,
    };
    httpd_register_uri_handler(httpd, &config_get);

    httpd_uri_t config_update = {
        .uri = "/api/wifi/config",
        .method = HTTP_POST,
        .handler = api_wifi_sta_config_update,
    };
    httpd_register_uri_handler(httpd, &config_update);
}
