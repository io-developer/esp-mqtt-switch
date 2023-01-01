#ifndef app_gpio_h
#define app_gpio_h


#include <string.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_system.h"
#include "lwip/sys.h"


#define APP_GPIO_LED_PIN_NUM GPIO_NUM_2
#define APP_GPIO_LED_PIN_MASK GPIO_Pin_2

#define APP_GPIO_RELAY_PIN_NUM GPIO_NUM_0
#define APP_GPIO_RELAY_PIN_MASK GPIO_Pin_0


void app_gpio_setup();
void app_gpio_test();

void app_gpio_led_on(bool on);
void app_gpio_led_blink(uint32_t interval_ms, uint8_t times);
void app_gpio_led_blink_us(uint32_t interval_us, uint8_t times);

void app_gpio_relay_on(bool on);


#endif