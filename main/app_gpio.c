#include "app_gpio.h"


static const char* TAG = "APP_GPIO";

static gpio_config_t app_gpio_led_conf = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = APP_GPIO_LED_PIN_MASK,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_ENABLE,
};

static gpio_config_t app_gpio_relay_conf = {
    .intr_type = GPIO_INTR_DISABLE,
    .mode = GPIO_MODE_OUTPUT,
    .pin_bit_mask = APP_GPIO_RELAY_PIN_MASK,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .pull_up_en = GPIO_PULLUP_ENABLE,
};


void app_gpio_setup()
{
    ESP_LOGI(TAG, "Setting up");

    ESP_ERROR_CHECK(gpio_config(&app_gpio_led_conf));
    ESP_ERROR_CHECK(gpio_config(&app_gpio_relay_conf));

    app_gpio_led_on(false);
    app_gpio_relay_on(false);
}

void app_gpio_test()
{
    ESP_LOGI(TAG, "Test: LED blinking");
    app_gpio_led_on(false);
    app_gpio_led_blink_us(50e3, 10);

    ESP_LOGI(TAG, "Test: Relay check");
    for (int i = 0; i < 3; i++) {
        app_gpio_relay_on(true);
        usleep(250e3);
        app_gpio_relay_on(false);
        usleep(250e3);
    }
}

void app_gpio_led_on(bool on)
{
    ESP_LOGD(TAG, on ? "LED -> ON" : "LED -> OFF");
    ESP_ERROR_CHECK(gpio_set_level(APP_GPIO_LED_PIN_NUM, on ? 0 : 1));
}

void app_gpio_led_blink(uint32_t interval_ms, uint8_t times)
{
    ESP_LOGD(TAG, "LED blink %d ms for %d times", interval_ms, times);
    for (int i = 0; i < times; i++) {
        app_gpio_led_on(true);
        vTaskDelay((interval_ms / 2) / portTICK_PERIOD_MS);
        app_gpio_led_on(false);
        vTaskDelay((interval_ms / 2) / portTICK_PERIOD_MS);
    }
}

void app_gpio_led_blink_us(uint32_t interval_us, uint8_t times)
{
    ESP_LOGD(TAG, "LED blink %d us for %d times", interval_us, times);
    for (int i = 0; i < times; i++) {
        app_gpio_led_on(true);
        usleep(interval_us >> 1);
        app_gpio_led_on(false);
        usleep(interval_us >> 1);
    }
}

void app_gpio_relay_on(bool on)
{
    ESP_LOGI(TAG, on ? "Relay -> ON" : "Relay -> OFF");
    ESP_ERROR_CHECK(gpio_set_level(APP_GPIO_RELAY_PIN_NUM, on ? 0 : 1));
}

