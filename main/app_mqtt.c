#include "app_mqtt.h"


static const char* TAG = "APP_MQTT";

static esp_mqtt_client_handle_t app_mqtt_client;
static bool app_mqtt_connected = false;


static int8_t app_mqtt_handle_msg_switch(
    const esp_mqtt_event_handle_t event,
    const app_mqtt_config_topic_t* expected,
    const app_switch_state_t flag
) {
    if (strncmp(expected->payload_true, event->data, event->data_len) == 0) {
        ESP_LOGI(TAG, "  data TRUE");
        app_switch_state_add(flag);
        return 1;
    }
    if (strncmp(expected->payload_false, event->data, event->data_len) == 0) {
        ESP_LOGI(TAG, "  data FALSE");
        app_switch_state_sub(flag);
        return 0;
    }

    ESP_LOGI(TAG, "  data UNKNOWN");
    return -1;
}

static void app_mqtt_on_event_cb(esp_mqtt_event_handle_t event)
{
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

            app_mqtt_connected = true;

            app_mqtt_subscribe(app_mqtt_config->topic_on.path);
            app_mqtt_subscribe(app_mqtt_config->topic_forceon.path);
            app_mqtt_subscribe(app_mqtt_config->topic_forceoff.path);

            app_mqtt_publish(
                app_mqtt_config->topic_availability.path,
                app_mqtt_config->topic_availability.payload_true
            );
            app_mqtt_publish_states(APP_SWITCH_STATE_ALL);

            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED. Retrying");
            app_mqtt_connected = false;
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d",
                event->msg_id
            );
            // msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            // ESP_LOGI(APP_MQTT_TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);

            app_switch_state_t prev_state = app_switch_state_get();

            if (strncmp(app_mqtt_config->topic_on.path, event->topic, event->topic_len) == 0) {
                app_mqtt_handle_msg_switch(
                    event,
                    &app_mqtt_config->topic_on,
                    APP_SWITCH_STATE_ON
                );
            } else if (strncmp(app_mqtt_config->topic_forceon.path, event->topic, event->topic_len) == 0) {
                app_mqtt_handle_msg_switch(
                    event,
                    &app_mqtt_config->topic_forceon,
                    APP_SWITCH_STATE_FORCEON
                );
            } else if (strncmp(app_mqtt_config->topic_forceoff.path, event->topic, event->topic_len) == 0) {
                app_mqtt_handle_msg_switch(
                    event,
                    &app_mqtt_config->topic_forceoff,
                    APP_SWITCH_STATE_FORCEOFF
                );
            }

            app_switch_state_t updated_state_flags = app_switch_state_diff_to(prev_state);
            app_mqtt_publish_states(updated_state_flags);

            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

static void app_mqtt_on_event(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    app_mqtt_on_event_cb(event_data);
}


bool app_mqtt_is_connected()
{
    return app_mqtt_connected;
}

void app_mqtt_setup()
{
    ESP_LOGI(TAG, "Setting up");

    app_mqtt_connected = false;

    app_mqtt_config_setup();

    esp_mqtt_client_config_t client_conf = {
        .uri = app_mqtt_config->uri,
        .username = app_mqtt_config->username,
        .password = app_mqtt_config->password,
        .client_id = app_mqtt_config->client_id,
        .lwt_topic = app_mqtt_config->topic_availability.path,
        .lwt_msg = app_mqtt_config->topic_availability.payload_false,
        .lwt_qos = app_mqtt_config->lwt_qos,
        .lwt_retain = app_mqtt_config->lwt_retain,
        .keepalive = APP_MQTT_KEEPALIVE,
        .disable_auto_reconnect = false,
        .reconnect_timeout_ms = APP_MQTT_RECONNECT_TIMEOUT_MS,
    };
    app_mqtt_client = esp_mqtt_client_init(&client_conf);

    ESP_ERROR_CHECK(esp_mqtt_client_register_event(app_mqtt_client, ESP_EVENT_ANY_ID, app_mqtt_on_event, NULL));
}

void app_mqtt_up()
{
    ESP_LOGI(TAG, "Going up..");
    ESP_ERROR_CHECK(esp_mqtt_client_start(app_mqtt_client));
}

void app_mqtt_down()
{
    ESP_LOGI(TAG, "Going down..");
    ESP_ERROR_CHECK(esp_mqtt_client_stop(app_mqtt_client));
    app_mqtt_connected = false;
}

void app_mqtt_subscribe(const char* topic)
{
    int msg_id = esp_mqtt_client_subscribe(
        app_mqtt_client,
        topic,
        app_mqtt_config->subscribe_qos
    );
    ESP_LOGI(TAG, "subscribe, msg_id=%d, topic=%s", msg_id, topic);
}

void app_mqtt_publish(const char* topic, const char* payload)
{
    int msg_id = esp_mqtt_client_publish(
        app_mqtt_client,
        topic,
        payload,
        0,
        app_mqtt_config->publish_qos,
        app_mqtt_config->publish_retain
    );
    ESP_LOGI(TAG, "publish msg_id=%d, topic=%s, payload='%s'", msg_id, topic, payload);
}

void app_mqtt_publish_states(app_switch_state_t updated)
{
    app_switch_state_t current = app_switch_state_get();

    if (updated & APP_SWITCH_STATE_ON) {
        app_mqtt_publish(app_mqtt_config->topic_state.path,
            (current & APP_SWITCH_STATE_ON)
                ? app_mqtt_config->topic_state.payload_true
                : app_mqtt_config->topic_state.payload_false
        );
    }
    if (updated & APP_SWITCH_STATE_FORCEON) {
        app_mqtt_publish(app_mqtt_config->topic_forceon_state.path,
            (current & APP_SWITCH_STATE_FORCEON)
                ? app_mqtt_config->topic_forceon_state.payload_true
                : app_mqtt_config->topic_forceon_state.payload_false
        );
    }
    if (updated & APP_SWITCH_STATE_FORCEOFF) {
        app_mqtt_publish(app_mqtt_config->topic_forceoff_state.path,
            (current & APP_SWITCH_STATE_FORCEOFF)
                ? app_mqtt_config->topic_forceon_state.payload_true
                : app_mqtt_config->topic_forceon_state.payload_false
        );
    }
}
