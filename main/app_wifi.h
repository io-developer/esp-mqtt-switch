#ifndef app_wifi_h
#define app_wifi_h


#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"

#include "app_util.h"


#define APP_WIFI_DEFAULT_AUTH_MODE WIFI_AUTH_WPA2_PSK
#define APP_WIFI_DEFAULT_AP_PASS "esp_admin"


/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1


int app_wifi_connect_attempt;

wifi_init_config_t app_wifi_init_config;
wifi_config_t app_wifi_sta_config;
wifi_config_t app_wifi_ap_config;

/* FreeRTOS event group to signal when we are connected*/
EventGroupHandle_t app_wifi_event_group;


void app_wifi_setup();

bool app_wifi_has_ip();

void app_wifi_sta_up();
bool app_wifi_sta_is_connected();
bool app_wifi_sta_wait_connected(uint32_t timeout_ms);

void app_wifi_ap_up();

void app_wifi_down();


#endif