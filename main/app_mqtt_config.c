#include "app_mqtt_config.h"


static const char* TAG = "APP_MQTT_CONFIG";


void app_mqtt_config_setup()
{
    ESP_LOGI(TAG, "Setting up...");

    app_mqtt_config = malloc(sizeof(app_mqtt_config_t));
    app_mqtt_config_load(app_mqtt_config);

    ESP_LOGI(TAG, "Current config:");
    app_mqtt_config_print(app_mqtt_config);
}

const app_mqtt_config_t* app_mqtt_config_get()
{
    return app_mqtt_config;
}


void app_mqtt_config_load(app_mqtt_config_t* dst)
{
    ESP_LOGW(TAG, "Config loading...");

    size_t blob_size = 0;
    esp_err_t err = nvs_get_blob(app_nvs_get_handle(), APP_MQTT_CONFIG_NVS_KEY, NULL, &blob_size);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG, "NVS value '%s' is not initialized! Creating default", APP_MQTT_CONFIG_NVS_KEY);
        app_mqtt_config_set_default(dst);
        return;
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS Error (%s) pre-reading! Blob size: %d", esp_err_to_name(err), blob_size);
        ESP_ERROR_CHECK(err);
        return;
    }
    if (blob_size != sizeof(app_mqtt_config_t)) {
        ESP_LOGE(TAG, "Different sizes!");
        ESP_LOGE(TAG, "  blob size: %d", blob_size);
        ESP_LOGE(TAG, "  sizeof config: %d", sizeof(app_mqtt_config_t));
        ESP_LOGE(TAG, "  using default config..");
        app_mqtt_config_set_default(dst);
        return;
    }

    err = nvs_get_blob(app_nvs_get_handle(), APP_MQTT_CONFIG_NVS_KEY, dst, &blob_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS Error (%s) reading! Blob size: %d", esp_err_to_name(err), blob_size);
        ESP_ERROR_CHECK(err);
        return;
    }

    ESP_LOGW(TAG, "Config loaded!");
}

void app_mqtt_config_save(app_mqtt_config_t* conf)
{
    ESP_LOGW(TAG, "Config saving. Sizeof: %d", sizeof(app_mqtt_config_t));

    nvs_handle h = app_nvs_get_handle();
    size_t size = sizeof(app_mqtt_config_t);

    ESP_ERROR_CHECK(nvs_set_blob(h, APP_MQTT_CONFIG_NVS_KEY, conf, size));
    ESP_ERROR_CHECK(nvs_commit(h));

    ESP_LOGW(TAG, "Config saved! Size: %d", size);
}

void app_mqtt_config_set_default(app_mqtt_config_t* dst)
{
    const char* uid = app_util_board_id_short();

    memset(dst->uri, 0, sizeof(dst->uri));
    sprintf(dst->uri, "mqtt://localhost");

    memset(dst->username, 0, sizeof(dst->username));
    sprintf(dst->username, "guest");

    memset(dst->password, 0, sizeof(dst->password));
    sprintf(dst->password, "guest");

    memset(dst->client_id, 0, sizeof(dst->client_id));
    sprintf(dst->client_id, uid);
    
    dst->lwt_qos = 1,
    dst->lwt_retain = 1,
    dst->publish_qos = 1,
    dst->publish_retain = 1,
    dst->subscribe_qos = 1,

    app_mqtt_config_topic_set_default(
        &dst->topic_availability,
        "switch/%s/availability",
        uid
    );
    app_mqtt_config_topic_set_default(
        &dst->topic_state,
        "switch/%s/state",
        uid
    );
    app_mqtt_config_topic_set_default(
        &dst->topic_on,
        "switch/%s/on",
        uid
    );
    app_mqtt_config_topic_set_default(
        &dst->topic_on_state,
        "switch/%s/on_state",
        uid
    );
    app_mqtt_config_topic_set_default(
        &dst->topic_forceon,
        "switch/%s/forceon",
        uid
    );
    app_mqtt_config_topic_set_default(
        &dst->topic_forceon_state,
        "switch/%s/forceon_state",
        uid
    );
    app_mqtt_config_topic_set_default(
        &dst->topic_forceoff,
        "switch/%s/forceoff",
        uid
    );
    app_mqtt_config_topic_set_default(
        &dst->topic_forceoff_state,
        "switch/%s/forceoff_state",
        uid
    );
}

void app_mqtt_config_topic_set_default(app_mqtt_config_topic_t* dst, const char* fmt, const char* uid)
{
    memset(dst->payload_true, 0, sizeof(dst->payload_true));
    sprintf(dst->payload_true, APP_MQTT_CONFIG_DEFAULT_PAYLOAD_TRUE);

    memset(dst->payload_false, 0, sizeof(dst->payload_false));
    sprintf(dst->payload_false, APP_MQTT_CONFIG_DEFAULT_PAYLOAD_FALSE);

    memset(dst->path, 0, sizeof(dst->path));
    sprintf(dst->path, fmt, uid);
}

void app_mqtt_config_print(const app_mqtt_config_t* conf)
{
    printf("\turi: '%s'\n", conf->uri);
    printf("\tusername: '%s'\n", conf->username);
    printf("\tpassword: '%s'\n", conf->password);
    printf("\tclient_id: '%s'\n", conf->client_id);
    printf("\tlwt_qos: %d\n", conf->lwt_qos);
    printf("\tlwt_retain: %d\n", conf->lwt_retain);
    printf("\tpublish_qos: %d\n", conf->publish_qos);
    printf("\tpublish_retain: %d\n", conf->publish_retain);
    printf("\tsubscribe_qos: %d\n", conf->subscribe_qos);

    printf("\ttopic_availability: \n");
    app_mqtt_config_topic_print(&conf->topic_availability);

    printf("\ttopic_state:\n");
    app_mqtt_config_topic_print(&conf->topic_state);

    printf("\ttopic_on:\n");
    app_mqtt_config_topic_print(&conf->topic_on);

    printf("\ttopic_on_state:\n");
    app_mqtt_config_topic_print(&conf->topic_on_state);

    printf("\ttopic_forceon:\n");
    app_mqtt_config_topic_print(&conf->topic_forceon);

    printf("\ttopic_forceon_state:\n");
    app_mqtt_config_topic_print(&conf->topic_forceon_state);

    printf("\ttopic_forceoff:\n");
    app_mqtt_config_topic_print(&conf->topic_forceoff);

    printf("\ttopic_forceoff_state:\n");
    app_mqtt_config_topic_print(&conf->topic_forceoff_state);

    fflush(stdout);
}

void app_mqtt_config_topic_print(const app_mqtt_config_topic_t* t)
{
    printf("\t\tpath: '%s'\n", t->path);
    printf("\t\tpayload_true: '%s'\n", t->payload_true);
    printf("\t\tpayload_false: '%s'\n", t->payload_false);
}
