#include "app_nvs.h"


static const char* TAG = "APP_NVS";


nvs_handle app_nvs_get_handle()
{
    return app_nvs_handle;
}

void app_nvs_setup()
{
    esp_err_t nvs_err = nvs_flash_init();
    if (nvs_err == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvs_err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(nvs_err);

    ESP_ERROR_CHECK(nvs_open(APP_NVS_NAME, NVS_READWRITE, &app_nvs_handle));
    ESP_LOGW(TAG, "nvs handle: %d", app_nvs_handle);
}

int32_t app_nvs_get_int32(const char* key)
{
    int32_t val = 0;
    esp_err_t err = nvs_get_i32(app_nvs_handle, key, &val);
    switch (err) {
        case ESP_OK:
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGW("NVS value '%s' is not initialized yet!", key);
            break;
        default :
            ESP_LOGE("NVS Error (%s) reading!", esp_err_to_name(err));
    }
    return val;
}

int32_t app_nvs_set_int32(const char* key, int32_t val)
{
    ESP_ERROR_CHECK(nvs_set_i32(app_nvs_handle, key, val));
    ESP_ERROR_CHECK(nvs_commit(app_nvs_handle));
    return val;
}

int32_t app_nvs_incr_int32(const char* key, int32_t step)
{
    return app_nvs_set_int32(key, app_nvs_get_int32(key) + step);
}
