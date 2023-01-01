#ifndef app_mqtt_h
#define app_mqtt_h


#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "esp_system.h"

#include "mqtt_client.h"

#include "app_mqtt_config.h"
#include "app_switch.h"
#include "app_util.h"


#define APP_MQTT_NVS_CONFIG_KEY "mqtt_config"

#define APP_MQTT_DEFAULT_PAYLOAD_TRUE "ON"
#define APP_MQTT_DEFAULT_PAYLOAD_FALSE "OFF"


void app_mqtt_setup();

bool app_mqtt_is_connected();

void app_mqtt_up();
void app_mqtt_down();

void app_mqtt_subscribe(const char* topic);
void app_mqtt_publish(const char* topic, const char* payload);

void app_mqtt_publish_states(app_switch_state_t updated);


#endif