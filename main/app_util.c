#include "app_util.h"


static uint8_t _mac[2][6];
static bool _mac_read = false;

static char _mac_str[2][16] = {"", ""};

static char _board_id[32] = "";
static char _board_id_short[32] = "";


void app_util_mac_read()
{
    ESP_ERROR_CHECK(esp_read_mac(_mac[ESP_MAC_WIFI_STA], ESP_MAC_WIFI_STA));
    ESP_ERROR_CHECK(esp_read_mac(_mac[ESP_MAC_WIFI_SOFTAP], ESP_MAC_WIFI_SOFTAP));
    _mac_read = true;
}

const char* app_util_mac_str(esp_mac_type_t type)
{
    if (!_mac_read) {
        app_util_mac_read();
    }
    char* outstr = _mac_str[(uint8_t)type];
    if ((uint8_t)outstr[0] == 0) {
        uint8_t* mac = _mac[(uint8_t)type];
        sprintf(outstr, "%x:%x:%x:%x:%x:%x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }
    return outstr;
}

const char* app_util_board_id()
{
    if (!_mac_read) {
        app_util_mac_read();
    }
    if ((uint8_t)_board_id[0] == 0) {
        uint8_t* mac = _mac[(uint8_t)ESP_MAC_WIFI_STA];
        sprintf(_board_id, "esp-%x%x%x%x%x%x",
            mac[0],
            mac[1],
            mac[2],
            mac[3],
            mac[4],
            mac[5]
        );
    }
    return _board_id;
}

const char* app_util_board_id_short()
{
    if (!_mac_read) {
        app_util_mac_read();
    }
    if ((uint8_t)_board_id_short[0] == 0) {
        uint8_t* mac = _mac[(uint8_t)ESP_MAC_WIFI_STA];
        sprintf(_board_id_short, "esp-%x%x%x",
            mac[3],
            mac[4],
            mac[5]
        );
    }
    return _board_id_short;
}


cJSON* app_util_json_get_item(const cJSON* json, const char* key)
{
    if (json == NULL) {
        return NULL;
    }
    if (!cJSON_HasObjectItem(json, key)) {
        return NULL;
    }
    return cJSON_GetObjectItemCaseSensitive(json, key);
}

const char* app_util_json_get_str(const cJSON* json, const char* key, const char* def)
{
    if (json == NULL) {
        return def;
    }
    if (!cJSON_HasObjectItem(json, key)) {
        return def;
    }
    const cJSON* item = cJSON_GetObjectItemCaseSensitive(json, key);
    if (!cJSON_IsString(item)) {
        return def;
    }
    return item->valuestring;
}

int app_util_json_get_int(const cJSON* json, const char* key, int def)
{
    if (json == NULL) {
        return def;
    }
    if (!cJSON_HasObjectItem(json, key)) {
        return def;
    }
    const cJSON* item = cJSON_GetObjectItemCaseSensitive(json, key);
    if (!cJSON_IsNumber(item)) {
        return def;
    }
    return item->valueint;
}

int app_util_json_get_double(const cJSON* json, const char* key, double def)
{
    if (json == NULL) {
        return def;
    }
    if (!cJSON_HasObjectItem(json, key)) {
        return def;
    }
    const cJSON* item = cJSON_GetObjectItemCaseSensitive(json, key);
    if (!cJSON_IsNumber(item)) {
        return def;
    }
    return item->valuedouble;
}
