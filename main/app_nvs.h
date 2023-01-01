#ifndef app_nvs_h
#define app_nvs_h


#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"


#define APP_NVS_NAME "storage"


nvs_handle app_nvs_handle;


void app_nvs_setup();

nvs_handle app_nvs_get_handle();

int32_t app_nvs_get_int32(const char* key);
int32_t app_nvs_set_int32(const char* key, int32_t val);
int32_t app_nvs_incr_int32(const char* key, int32_t step);


#endif