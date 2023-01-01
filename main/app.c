#include "app.h"


static const char* TAG = "APP";


static void app_boot_complete_task(void *arg)
{
    vTaskDelay(APP_BOOT_COMPLETE_TIMEOUT / portTICK_PERIOD_MS);

    ESP_LOGW(TAG, "BOOT COMPLETE. Interruption counter erased");
    app_nvs_set_int32(APP_BOOT_INTERRUPTED_COUNTER_KEY, 0);

    vTaskDelete(NULL);
}

void app_setup()
{
    ESP_LOGI(TAG, "Startup..");
    ESP_LOGI(TAG, "Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "IDF version: %s", esp_get_idf_version());

    printf("Test printf: decimal=%04d, float=%6.2f\n", 42, 12345.6789);
    fflush(stdout);

    char buf[256];
    sprintf(buf, "Test sprintf: decimal=%04d, float=%6.2f\n", 42, 12345.6789);
    printf("%s\n", buf);
    fflush(stdout);

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_LOGI(TAG, "WiFi access point MAC: '%s'", app_util_mac_str(ESP_MAC_WIFI_SOFTAP));
    ESP_LOGI(TAG, "WiFi station MAC: '%s'", app_util_mac_str(ESP_MAC_WIFI_STA));
    
    app_nvs_setup();

    app_boot_counter = app_nvs_incr_int32(APP_BOOT_COUNTER_KEY, 1);
    app_boot_primary_counter = app_nvs_get_int32(APP_BOOT_PRIMARY_COUNTER_KEY);
    app_boot_secondary_counter = app_nvs_get_int32(APP_BOOT_SECONDARY_COUNTER_KEY);
    app_boot_interrupted_counter = app_nvs_incr_int32(APP_BOOT_INTERRUPTED_COUNTER_KEY, 1);
    xTaskCreate(app_boot_complete_task, "app_boot_complete_task", 2048, NULL, 10, NULL);

    ESP_LOGI(TAG, "Counters:");
    ESP_LOGI(TAG, "  boot: %d", app_boot_counter);
    ESP_LOGI(TAG, "  boot primary: %d", app_boot_primary_counter);
    ESP_LOGI(TAG, "  boot secondary: %d", app_boot_secondary_counter);
    ESP_LOGI(TAG, "  boot interrupted: %d", app_boot_interrupted_counter);

    app_gpio_setup();
    app_switch_setup();
}

void app_main()
{
    app_setup();

    if (app_boot_interrupted_counter > APP_BOOT_INTTERRUPTED_THRESHOLD) {
        app_main_secondary();
    } else {
        app_main_primary();
    }
}


// -------------- PRIMARY MODE --------------------

static void app_main_primary_watchdog_task(void *arg)
{
    ESP_LOGW(TAG, "[WATCHDOG] Task started");

    uint16_t fail_counter = 0;
    for (;;) {
        if (!app_wifi_sta_is_connected() || !app_wifi_has_ip()) {
            fail_counter++;
            ESP_LOGW(TAG, "[WATCHDOG] WiFi failure. Increasing counter: %d", fail_counter);
        } else if (!app_mqtt_is_connected()) {
            fail_counter++;
            ESP_LOGW(TAG, "[WATCHDOG] MQTT failure. Increasing counter: %d", fail_counter);
        } else if (fail_counter > 0) {
            fail_counter = 0;
            ESP_LOGW(TAG, "[WATCHDOG] No failure. Resetting counter: %d", fail_counter);
        }
        
        if (fail_counter > APP_MAIN_PRIMARY_WATCHDOG_FAILURE_LIMIT) {
            ESP_LOGW(TAG, "[WATCHDOG] Reached failure limit (%d). Restarting...", fail_counter);
            esp_restart();
        }
        
        vTaskDelay(APP_MAIN_PRIMARY_WATCHDOG_INTERVAL / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

static void app_main_primary_status_task(void *arg)
{
    ESP_LOGW(TAG, "app_main_primary_status_task started");

    vTaskDelay(APP_MAIN_PRIMARY_STATUS_DEFAULT_INTERVAL / portTICK_PERIOD_MS);

    for (;;) {
        uint32_t interval_ms = APP_MAIN_PRIMARY_STATUS_DEFAULT_INTERVAL;
        if (!app_wifi_sta_is_connected()) {
            ESP_LOGI(TAG, "! app_wifi_sta_is_connected");
            app_gpio_led_blink(
                APP_MAIN_PRIMARY_STATUS_NOWIFI_LED_PERIOD,
                APP_MAIN_PRIMARY_STATUS_NOWIFI_LED_TIMES
            );
        } else if (!app_wifi_has_ip()) {
            ESP_LOGI(TAG, "! app_wifi_has_ip");
            app_gpio_led_blink(
                APP_MAIN_PRIMARY_STATUS_NOIP_LED_PERIOD,
                APP_MAIN_PRIMARY_STATUS_NOIP_LED_TIMES
            );
        } else if (!app_mqtt_is_connected()) {
            ESP_LOGI(TAG, "! app_mqtt_is_connected");
            app_gpio_led_blink(
                APP_MAIN_PRIMARY_STATUS_NOMQTT_LED_PERIOD,
                APP_MAIN_PRIMARY_STATUS_NOMQTT_LED_TIMES
            );
        } else {
            printf(APP_MAIN_PRIMARY_STATUS_OK_STDOUT);
            fflush(stdout);

            app_gpio_led_blink(
                APP_MAIN_PRIMARY_STATUS_OK_LED_PERIOD,
                APP_MAIN_PRIMARY_STATUS_OK_LED_TIMES
            );
            interval_ms = APP_MAIN_PRIMARY_STATUS_OK_INTERVAL;
        }
        vTaskDelay(interval_ms / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void app_main_primary()
{
    ESP_LOGW(TAG, "PRIMARY MODE STARTED");

    app_nvs_incr_int32(APP_BOOT_PRIMARY_COUNTER_KEY, 1);

    app_wifi_setup();
    app_mqtt_setup();
    app_api_setup();

    xTaskCreate(app_main_primary_status_task, "app_main_primary_status_task", 2048, NULL, 10, NULL);

    app_wifi_sta_up();
    if (!app_wifi_sta_wait_connected(APP_MAIN_PRIMARY_STA_UP_TIMEOUT)) {
        ESP_LOGW(TAG, "Wifi failed. Restarting ESP");
        esp_restart();
    }

    xTaskCreate(app_main_primary_watchdog_task, "app_main_primary_watchdog_task", 2048, NULL, 10, NULL);

    app_mqtt_up();
}


// -------------- SECONDARY MODE --------------------

static void app_main_secondary_timeout_task(void *arg)
{
    ESP_LOGW(TAG, "SECONDARY MODE TIMEOUT after: %d ms", APP_MAIN_SECONDARY_TIMEOUT_MS);
    vTaskDelay(APP_MAIN_SECONDARY_TIMEOUT_MS / portTICK_PERIOD_MS);

    ESP_LOGW(TAG, "SECONDARY MODE TIMEOUT REACHED. Restarting...");
    esp_restart();

    vTaskDelete(NULL);
}

static void app_main_secondary_status_task(void *arg)
{
    ESP_LOGW(TAG, "app_main_secondary_status_task started");

    for (;;) {
        app_gpio_led_blink(
            APP_MAIN_SECONDARY_STATUS_OK_LED_PERIOD,
            APP_MAIN_SECONDARY_STATUS_OK_LED_TIMES
        );
    }
    vTaskDelete(NULL);
}

void app_main_secondary()
{
    ESP_LOGW(TAG, "SECONDARY MODE STARTED");

    //ESP_ERROR_CHECK(nvs_flash_erase());
    //ESP_ERROR_CHECK(nvs_flash_init());
    
    app_nvs_incr_int32(APP_BOOT_SECONDARY_COUNTER_KEY, 1);

    xTaskCreate(app_main_secondary_timeout_task, "app_main_secondary_timeout_task", 2048, NULL, 10, NULL);
    xTaskCreate(app_main_secondary_status_task, "app_main_secondary_status_task", 2048, NULL, 10, NULL);

    app_wifi_setup();
    app_wifi_ap_up();

    app_api_setup();
    app_api_up();
}
