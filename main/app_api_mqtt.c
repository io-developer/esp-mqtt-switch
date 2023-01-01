#include "app_api.h"


static cJSON* config_topic_to_json(const app_mqtt_config_topic_t* conf)
{
    cJSON* json = cJSON_CreateObject();

    cJSON_AddStringToObject(json, "path", conf->path);
    cJSON_AddStringToObject(json, "payload_true", conf->payload_true);
    cJSON_AddStringToObject(json, "payload_false", conf->payload_false);

    return json;
}

static cJSON* config_to_json(const app_mqtt_config_t* conf)
{
    cJSON* json = cJSON_CreateObject();

    cJSON_AddStringToObject(json, "uri", conf->uri);
    cJSON_AddStringToObject(json, "username", conf->username);
    cJSON_AddStringToObject(json, "password", conf->password);
    cJSON_AddStringToObject(json, "client_id", conf->client_id);

    cJSON_AddNumberToObject(json, "lwt_qos", conf->lwt_qos);
    cJSON_AddNumberToObject(json, "lwt_retain", conf->lwt_retain);
    cJSON_AddNumberToObject(json, "publish_qos", conf->publish_qos);
    cJSON_AddNumberToObject(json, "publish_retain", conf->publish_retain);
    cJSON_AddNumberToObject(json, "subscribe_qos", conf->subscribe_qos);

    cJSON_AddItemToObject(json, "topic_availability", config_topic_to_json(
        &conf->topic_availability
    ));
    cJSON_AddItemToObject(json, "topic_state", config_topic_to_json(
        &conf->topic_state
    ));
    cJSON_AddItemToObject(json, "topic_on", config_topic_to_json(
        &conf->topic_on
    ));
    cJSON_AddItemToObject(json, "topic_on_state", config_topic_to_json(
        &conf->topic_on_state
    ));
    cJSON_AddItemToObject(json, "topic_forceon", config_topic_to_json(
        &conf->topic_forceon
    ));
    cJSON_AddItemToObject(json, "topic_forceon_state", config_topic_to_json(
        &conf->topic_forceon_state
    ));
    cJSON_AddItemToObject(json, "topic_forceoff", config_topic_to_json(
        &conf->topic_forceoff
    ));
    cJSON_AddItemToObject(json, "topic_forceoff_state", config_topic_to_json(
        &conf->topic_forceoff_state
    ));

    return json;
}


static void config_topic_from_json(const cJSON *json, app_mqtt_config_topic_t *dst, const app_mqtt_config_topic_t *src)
{
    memset(dst->path, 0, sizeof(dst->path));
    sprintf(dst->path, "%s", app_util_json_get_str(json, "path", src->path));

    memset(dst->payload_true, 0, sizeof(dst->payload_true));
    sprintf(dst->payload_true, "%s", app_util_json_get_str(json, "payload_true", src->payload_true));

    memset(dst->payload_false, 0, sizeof(dst->payload_false));
    sprintf(dst->payload_false, "%s", app_util_json_get_str(json, "payload_false", src->payload_false));
}

static void config_from_json(const cJSON *json, app_mqtt_config_t *dst, const app_mqtt_config_t *src)
{
    memset(dst->uri, 0, sizeof(dst->uri));
    sprintf(dst->uri, "%s", app_util_json_get_str(json, "uri", src->uri));

    memset(dst->username, 0, sizeof(dst->username));
    sprintf(dst->username, "%s", app_util_json_get_str(json, "username", src->username));

    memset(dst->password, 0, sizeof(dst->password));
    sprintf(dst->password, "%s", app_util_json_get_str(json, "password", src->password));

    memset(dst->client_id, 0, sizeof(dst->client_id));
    sprintf(dst->client_id, "%s", app_util_json_get_str(json, "client_id", src->client_id));

    dst->lwt_qos = app_util_json_get_int(json, "lwt_qos", src->lwt_qos);
    dst->lwt_retain = app_util_json_get_int(json, "lwt_retain", src->lwt_retain);
    dst->publish_qos = app_util_json_get_int(json, "publish_qos", src->publish_qos);
    dst->publish_retain = app_util_json_get_int(json, "publish_retain", src->publish_retain);
    dst->subscribe_qos = app_util_json_get_int(json, "subscribe_qos", src->subscribe_qos);

    if (cJSON_HasObjectItem(json, "topic_availability")) {
        config_topic_from_json(
            cJSON_GetObjectItemCaseSensitive(json, "topic_availability"),
            &dst->topic_availability,
            &src->topic_availability
        );
    }
    if (cJSON_HasObjectItem(json, "topic_state")) {
        config_topic_from_json(
            cJSON_GetObjectItemCaseSensitive(json, "topic_state"),
            &dst->topic_state,
            &src->topic_state
        );
    }
    if (cJSON_HasObjectItem(json, "topic_on")) {
        config_topic_from_json(
            cJSON_GetObjectItemCaseSensitive(json, "topic_on"),
            &dst->topic_on,
            &src->topic_on
        );
    }
    if (cJSON_HasObjectItem(json, "topic_on_state")) {
        config_topic_from_json(
            cJSON_GetObjectItemCaseSensitive(json, "topic_on_state"),
            &dst->topic_on_state,
            &src->topic_on_state
        );
    }
    if (cJSON_HasObjectItem(json, "topic_forceon")) {
        config_topic_from_json(
            cJSON_GetObjectItemCaseSensitive(json, "topic_forceon"),
            &dst->topic_forceon,
            &src->topic_forceon
        );
    }
    if (cJSON_HasObjectItem(json, "topic_forceon_state")) {
        config_topic_from_json(
            cJSON_GetObjectItemCaseSensitive(json, "topic_forceon_state"),
            &dst->topic_forceon_state,
            &src->topic_forceon_state
        );
    }
    if (cJSON_HasObjectItem(json, "topic_forceoff")) {
        config_topic_from_json(
            cJSON_GetObjectItemCaseSensitive(json, "topic_forceoff"),
            &dst->topic_forceoff,
            &src->topic_forceoff
        );
    }
    if (cJSON_HasObjectItem(json, "topic_forceoff_state")) {
        config_topic_from_json(
            cJSON_GetObjectItemCaseSensitive(json, "topic_forceoff_state"),
            &dst->topic_forceoff_state,
            &src->topic_forceoff_state
        );
    }
}


static esp_err_t api_mqtt_config_get(httpd_req_t *req)
{
    app_mqtt_config_t* conf = malloc(sizeof(app_mqtt_config_t));
    app_mqtt_config_load(conf);

    cJSON* json = config_to_json(conf);
    esp_err_t err = app_api_httpd_send_json(req, json);
    cJSON_Delete(json);

    return err;
}

static esp_err_t api_mqtt_config_update(httpd_req_t *req)
{
    char* content = NULL;
    cJSON *json = NULL;
    esp_err_t err = app_api_httpd_read_json(req, &content, &json);
    if (err != ESP_OK) {
        return err;
    }

    app_mqtt_config_t* src = malloc(sizeof(app_mqtt_config_t));
    app_mqtt_config_load(src);

    app_mqtt_config_t* dst = malloc(sizeof(app_mqtt_config_t));
    memcpy(dst, src, sizeof(app_mqtt_config_t));

    config_from_json(json, dst, src);
    app_mqtt_config_save(dst);

    cJSON* resp_json = config_to_json(dst);
    err = app_api_httpd_send_json(req, resp_json);

    cJSON_Delete(resp_json);
    cJSON_Delete(json);
    free(src);
    free(dst);

    return err;
}


void app_api_mqtt_register_endpoints(httpd_handle_t httpd)
{
    httpd_uri_t config_get = {
        .uri = "/api/mqtt/config",
        .method = HTTP_GET,
        .handler = api_mqtt_config_get,
    };
    httpd_register_uri_handler(httpd, &config_get);

    httpd_uri_t config_update = {
        .uri = "/api/mqtt/config",
        .method = HTTP_POST,
        .handler = api_mqtt_config_update,
    };
    httpd_register_uri_handler(httpd, &config_update);
}
