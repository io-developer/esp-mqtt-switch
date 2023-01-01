#ifndef app_mqtt_config_h
#define app_mqtt_config_h


#include <string.h>

#include "freertos/FreeRTOS.h"

#include "esp_log.h"
#include "esp_system.h"

#include "app_util.h"
#include "app_nvs.h"


#define APP_MQTT_CONFIG_NVS_KEY "mqtt_config"

#define APP_MQTT_CONFIG_DEFAULT_PAYLOAD_TRUE "ON"
#define APP_MQTT_CONFIG_DEFAULT_PAYLOAD_FALSE "OFF"


typedef struct {
    char path[128];
    char payload_true[16];
    char payload_false[16];
} app_mqtt_config_topic_t;

typedef struct {
    char uri[128];
    char username[16];
    char password[32];
    char client_id[16];

    uint8_t lwt_qos;
    uint8_t lwt_retain;

    uint8_t publish_qos;
    uint8_t publish_retain;

    uint8_t subscribe_qos;

    app_mqtt_config_topic_t topic_availability;
    app_mqtt_config_topic_t topic_state;

    app_mqtt_config_topic_t topic_on;
    app_mqtt_config_topic_t topic_on_state;

    app_mqtt_config_topic_t topic_forceon;
    app_mqtt_config_topic_t topic_forceon_state;

    app_mqtt_config_topic_t topic_forceoff;
    app_mqtt_config_topic_t topic_forceoff_state;

    char reserved[1];
} app_mqtt_config_t;


app_mqtt_config_t* app_mqtt_config;


void app_mqtt_config_setup();

void app_mqtt_config_load(app_mqtt_config_t* dst);
void app_mqtt_config_save(app_mqtt_config_t* conf);

const app_mqtt_config_t* app_mqtt_config_get();
void app_mqtt_config_set_default(app_mqtt_config_t* dst);
void app_mqtt_config_topic_set_default(app_mqtt_config_topic_t* dst, const char* fmt, const char* uid);

void app_mqtt_config_print(const app_mqtt_config_t* conf);
void app_mqtt_config_topic_print(const app_mqtt_config_topic_t* t);


#endif