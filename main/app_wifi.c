#include "app_wifi.h"


static const char* TAG = "APP_WIFI";

static bool _app_wifi_sta_is_connected;
static bool _app_wifi_has_ip;


bool app_wifi_sta_is_connected()
{
    return _app_wifi_sta_is_connected;
}

bool app_wifi_has_ip()
{
    return _app_wifi_has_ip;
}

static void app_wifi_on_wifi_event(
    void* arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void* event_data
) {
    ESP_LOGI(TAG, "WIFI_EVENT #%d", event_id);

    switch (event_id)
    {
        case WIFI_EVENT_STA_START: {
            ESP_LOGI(TAG,"WIFI_EVENT_STA_START");
            esp_wifi_connect();
            break;
        }
        case WIFI_EVENT_STA_CONNECTED: {
            ESP_LOGI(TAG,"WIFI_EVENT_STA_CONNECTED");
            _app_wifi_sta_is_connected = true;
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED: {
            ESP_LOGW(TAG,"WIFI_EVENT_STA_DISCONNECTED");
            _app_wifi_sta_is_connected = false;
            if (app_wifi_connect_attempt < CONFIG_ESP_MAXIMUM_RETRY) {
                ESP_ERROR_CHECK(esp_wifi_disconnect());
                ESP_ERROR_CHECK(esp_wifi_connect());
                app_wifi_connect_attempt++;
                ESP_LOGW(TAG, "Retry to connect to the AP");
            } else {
                xEventGroupSetBits(app_wifi_event_group, WIFI_FAIL_BIT);
                ESP_LOGE(TAG, "Connecting to the AP failed");
                //esp_restart();
            }
            break;
        }
        default:
            break;
    }
}

static void app_wifi_on_ip_event(
    void* arg,
    esp_event_base_t event_base,
    int32_t event_id,
    void* event_data
) {
    ESP_LOGI(TAG, "IP_EVENT #%d", event_id);

    switch (event_id)
    {
        case IP_EVENT_STA_GOT_IP: {
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(TAG, "IP_EVENT_GOT_IP: %s", ip4addr_ntoa(&event->ip_info.ip));
            _app_wifi_has_ip = true;
            app_wifi_connect_attempt = 0;
            xEventGroupSetBits(app_wifi_event_group, WIFI_CONNECTED_BIT);
            break;
        }
        case IP_EVENT_STA_LOST_IP: {
            ESP_LOGW(TAG, "IP_EVENT_LOST_IP");
            _app_wifi_has_ip = false;
            break;
        }
        default:
            break;
    }    
}

void app_wifi_setup_sta()
{
    wifi_sta_config_t* sta = &app_wifi_sta_config.sta;

    // app_wifi_sta_config.sta.ssid = CONFIG_ESP_WIFI_SSID;
    // app_wifi_sta_config.sta.password = CONFIG_ESP_WIFI_PASSWORD;
    sta->threshold.authmode = APP_WIFI_DEFAULT_AUTH_MODE;
}

void app_wifi_setup_ap()
{
    wifi_ap_config_t* ap = &app_wifi_ap_config.ap;

    ap->authmode = APP_WIFI_DEFAULT_AUTH_MODE;

    const char* ssid = app_util_board_id_short();
    const char* pass = APP_WIFI_DEFAULT_AP_PASS;

    memset(ap->ssid, 0, sizeof(ap->ssid));
    memcpy(ap->ssid, ssid, strlen(ssid));

    memset(ap->password, 0, sizeof(ap->password));
    memcpy(ap->password, pass, strlen(pass));
}

void app_wifi_setup()
{
    ESP_LOGI(TAG, "Setting up STA mode...");

    _app_wifi_sta_is_connected = false;
    _app_wifi_has_ip = false;

    app_wifi_connect_attempt = 0;
    app_wifi_event_group = xEventGroupCreate();

    wifi_init_config_t tmp_init_config = WIFI_INIT_CONFIG_DEFAULT();
    app_wifi_init_config = tmp_init_config;

    ESP_ERROR_CHECK(esp_wifi_init(&app_wifi_init_config));
    ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA, &app_wifi_sta_config));
    ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_AP, &app_wifi_ap_config));

    app_wifi_setup_sta();
    app_wifi_setup_ap();

    ESP_LOGW(TAG, "STA ssid: %s", app_wifi_sta_config.sta.ssid);
    ESP_LOGW(TAG, "STA password: %s", app_wifi_sta_config.sta.password);

    ESP_LOGW(TAG, "AP ssid: %s", app_wifi_ap_config.ap.ssid);
    ESP_LOGW(TAG, "AP password: %s", app_wifi_ap_config.ap.password);

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &app_wifi_on_wifi_event, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &app_wifi_on_ip_event, NULL));
}

void app_wifi_sta_up()
{
    ESP_LOGI(TAG, "STA Starting...");

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &app_wifi_sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "STA started");
}

bool app_wifi_sta_wait_connected(uint32_t timeout_ms)
{
    ESP_LOGI(TAG, "STA awaiting complete...");

    EventBits_t bits = xEventGroupWaitBits(
        app_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,
        pdFALSE,
        timeout_ms / portTICK_PERIOD_MS
    );

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to ap SSID: %s, PASS: %s", app_wifi_sta_config.sta.ssid, app_wifi_sta_config.sta.password);
        return true;
    }
    if (bits & WIFI_FAIL_BIT) {
        ESP_LOGW(TAG, "Failed to connect to SSID: %s, PASS: %s", app_wifi_sta_config.sta.ssid, app_wifi_sta_config.sta.password);
        return false;
    }

    ESP_LOGE(TAG, "UNEXPECTED EVENT");
    return false;
}

void app_wifi_ap_up()
{
    ESP_LOGI(TAG, "Starting AP...");

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &app_wifi_ap_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &app_wifi_sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "esp_wifi_start finished");
}

void app_wifi_down()
{
    ESP_LOGI(TAG, "Stopping..");

    _app_wifi_sta_is_connected = false;
    _app_wifi_has_ip = false;

    ESP_ERROR_CHECK(esp_wifi_stop());
}
