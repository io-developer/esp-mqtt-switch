#ifndef app_switch_h
#define app_switch_h


#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_system.h"

#include "app_gpio.h"


typedef enum {
    APP_SWITCH_STATE_ALL = -1,
    APP_SWITCH_STATE_NONE = 0,
    APP_SWITCH_STATE_ON = 1,
    APP_SWITCH_STATE_FORCEON =  2,
    APP_SWITCH_STATE_FORCEOFF = 4,
} app_switch_state_t;


void app_switch_setup();
void app_switch_update();

bool app_switch_state_to_relay_state(app_switch_state_t state);

app_switch_state_t app_switch_state_diff(app_switch_state_t a, app_switch_state_t b);
app_switch_state_t app_switch_state_diff_to(app_switch_state_t target);

app_switch_state_t app_switch_state_get();
void app_switch_state_set(app_switch_state_t state);
void app_switch_state_add(app_switch_state_t flag);
void app_switch_state_sub(app_switch_state_t flag);

int app_switch_state_str(char* outstr, app_switch_state_t state);


#endif