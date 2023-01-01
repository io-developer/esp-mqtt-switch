#ifndef app_util_h
#define app_util_h


#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "cJSON.h"


const char* app_util_mac_str(esp_mac_type_t type);
int app_util_mac_str_read(char* outstr, esp_mac_type_t type);

const char* app_util_board_id();
const char* app_util_board_id_short();

cJSON* app_util_json_get_item(const cJSON* json, const char* key);
const char* app_util_json_get_str(const cJSON* json, const char* key, const char* def);
int app_util_json_get_int(const cJSON* json, const char* key, int def);
int app_util_json_get_double(const cJSON* json, const char* key, double def);


#endif