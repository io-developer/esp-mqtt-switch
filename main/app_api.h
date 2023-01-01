#ifndef app_api_h
#define app_api_h


#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"

#include <esp_http_server.h>

#include "app_switch.h"
#include "app_wifi.h"
#include "app_mqtt_config.h"
#include "app_util.h"
#include "cJSON.h"


void app_api_setup();
bool app_api_up();

void app_api_wifi_register_endpoints(httpd_handle_t httpd);
void app_api_mqtt_register_endpoints(httpd_handle_t httpd);
void app_api_switch_register_endpoints(httpd_handle_t httpd);

esp_err_t app_api_httpd_read_json(httpd_req_t* req, char** out_content, cJSON** out_json);
esp_err_t app_api_httpd_send_json(httpd_req_t* req, cJSON* json);


#endif