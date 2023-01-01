#ifndef app_h
#define app_h


#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "app_gpio.h"
#include "app_nvs.h"
#include "app_wifi.h"
#include "app_mqtt.h"
#include "app_switch.h"
#include "app_api.h"


#define APP_BOOT_COUNTER_KEY "boot_num"
#define APP_BOOT_PRIMARY_COUNTER_KEY "boot_prim_num"
#define APP_BOOT_SECONDARY_COUNTER_KEY "boot_sec_num"
#define APP_BOOT_INTERRUPTED_COUNTER_KEY "boot_itrp_num"
#define APP_BOOT_COMPLETE_TIMEOUT 3000
#define APP_BOOT_INTTERRUPTED_THRESHOLD 3

#define APP_MAIN_PRIMARY_WATCHDOG_INTERVAL 1000
#define APP_MAIN_PRIMARY_WATCHDOG_FAILURE_LIMIT 30

#define APP_MAIN_PRIMARY_STA_UP_TIMEOUT 30000

#define APP_MAIN_PRIMARY_STATUS_OK_INTERVAL 10000
#define APP_MAIN_PRIMARY_STATUS_OK_LED_PERIOD 40
#define APP_MAIN_PRIMARY_STATUS_OK_LED_TIMES 1
#define APP_MAIN_PRIMARY_STATUS_OK_STDOUT "."
#define APP_MAIN_PRIMARY_STATUS_DEFAULT_INTERVAL 2000
#define APP_MAIN_PRIMARY_STATUS_NOWIFI_LED_PERIOD 1000
#define APP_MAIN_PRIMARY_STATUS_NOWIFI_LED_TIMES 3
#define APP_MAIN_PRIMARY_STATUS_NOIP_LED_PERIOD 500
#define APP_MAIN_PRIMARY_STATUS_NOIP_LED_TIMES 3
#define APP_MAIN_PRIMARY_STATUS_NOMQTT_LED_PERIOD 1000
#define APP_MAIN_PRIMARY_STATUS_NOMQTT_LED_TIMES 2

#define APP_MAIN_SECONDARY_TIMEOUT_MS 1800000

#define APP_MAIN_SECONDARY_STATUS_OK_LED_PERIOD 2000
#define APP_MAIN_SECONDARY_STATUS_OK_LED_TIMES 1


int32_t app_boot_counter;
int32_t app_boot_primary_counter;
int32_t app_boot_secondary_counter;
int32_t app_boot_interrupted_counter;


void app_setup();
void app_main();
void app_main_secondary();
void app_main_primary();


#endif