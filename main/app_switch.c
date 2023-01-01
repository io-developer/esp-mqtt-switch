#include "app_switch.h"


static const char* TAG = "APP_SWITCH";

static app_switch_state_t app_switch_state;


void app_switch_setup()
{
    ESP_LOGI(TAG, "Setting up...");

    app_switch_state = APP_SWITCH_STATE_NONE;

    app_switch_update();
}

void app_switch_update()
{
    bool on = app_switch_state_to_relay_state(app_switch_state);
    app_gpio_relay_on(on);

    ESP_LOGI(TAG, "Updated. State: %d, relay: %d", (int)app_switch_state, (int)on);
}

bool app_switch_state_to_relay_state(app_switch_state_t state)
{
    if (state & APP_SWITCH_STATE_FORCEOFF) {
        return false;
    }
    return state & (APP_SWITCH_STATE_ON | APP_SWITCH_STATE_FORCEON);
}

app_switch_state_t app_switch_state_diff(app_switch_state_t a, app_switch_state_t b)
{
    return (a | b) ^ (a & b);
}

app_switch_state_t app_switch_state_diff_to(app_switch_state_t target)
{
    return (target | app_switch_state) ^ (target & app_switch_state);
}

app_switch_state_t app_switch_state_get()
{
    return app_switch_state;
}

void app_switch_state_set(app_switch_state_t state)
{
    ESP_LOGI(TAG, "set state: %d", (int)state);

    app_switch_state = state;
    app_switch_update();
}

void app_switch_state_add(app_switch_state_t flag)
{
    app_switch_state_set(app_switch_state | flag);
}

void app_switch_state_sub(app_switch_state_t flag)
{
    app_switch_state_set((app_switch_state | flag) ^ flag);
}

int app_switch_state_str(char* outstr, app_switch_state_t state)
{
    return sprintf(
        outstr,
        "%s [%d: %sON %sFORCEON %sFORCEOFF]",
        app_switch_state_to_relay_state(state) ? "ON" : "OFF",
        (int)state,
        (state & APP_SWITCH_STATE_ON) ? "+" : "-",
        (state & APP_SWITCH_STATE_FORCEON) ? "+" : "-",
        (state & APP_SWITCH_STATE_FORCEOFF) ? "+" : "-"
    );
}