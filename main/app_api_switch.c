#include "app_api.h"


static esp_err_t api_switch_state_get(httpd_req_t* req)
{
    char resp[128];
    int len = app_switch_state_str(resp, app_switch_state_get());
    httpd_resp_send(req, resp, len);
    return ESP_OK;
}


static esp_err_t api_switch_on(httpd_req_t* req)
{
    app_switch_state_add(APP_SWITCH_STATE_ON);
    char resp[64];
    int len = app_switch_state_str(resp, app_switch_state_get());
    httpd_resp_send(req, resp, len);
    return ESP_OK;
}

static esp_err_t api_switch_off(httpd_req_t* req)
{
    app_switch_state_sub(APP_SWITCH_STATE_ON);
    char resp[64];
    int len = app_switch_state_str(resp, app_switch_state_get());
    httpd_resp_send(req, resp, len);
    return ESP_OK;
}


static esp_err_t api_switch_forceon_on(httpd_req_t* req)
{
    app_switch_state_add(APP_SWITCH_STATE_FORCEON);
    char resp[64];
    int len = app_switch_state_str(resp, app_switch_state_get());
    httpd_resp_send(req, resp, len);
    return ESP_OK;
}

static esp_err_t api_switch_forceon_off(httpd_req_t* req)
{
    app_switch_state_sub(APP_SWITCH_STATE_FORCEON);
    char resp[64];
    int len = app_switch_state_str(resp, app_switch_state_get());
    httpd_resp_send(req, resp, len);
    return ESP_OK;
}


static esp_err_t api_switch_forceoff_on(httpd_req_t* req)
{
    app_switch_state_add(APP_SWITCH_STATE_FORCEOFF);
    char resp[64];
    int len = app_switch_state_str(resp, app_switch_state_get());
    httpd_resp_send(req, resp, len);
    return ESP_OK;
}

static esp_err_t api_switch_forceoff_off(httpd_req_t* req)
{
    app_switch_state_sub(APP_SWITCH_STATE_FORCEOFF);
    char resp[64];
    int len = app_switch_state_str(resp, app_switch_state_get());
    httpd_resp_send(req, resp, len);
    return ESP_OK;
}


void app_api_switch_register_endpoints(httpd_handle_t httpd)
{
    httpd_uri_t state_get = {
        .uri = "/api/switch/state",
        .method = HTTP_GET,
        .handler = api_switch_state_get,
    };
    httpd_register_uri_handler(httpd, &state_get);

    httpd_uri_t on = {
        .uri = "/api/switch/on",
        .method = HTTP_POST,
        .handler = api_switch_on,
    };
    httpd_register_uri_handler(httpd, &on);

    httpd_uri_t off = {
        .uri = "/api/switch/off",
        .method = HTTP_POST,
        .handler = api_switch_off,
    };
    httpd_register_uri_handler(httpd, &off);

    httpd_uri_t forceon_on = {
        .uri = "/api/switch/forceon/on",
        .method = HTTP_POST,
        .handler = api_switch_forceon_on,
    };
    httpd_register_uri_handler(httpd, &forceon_on);

    httpd_uri_t forceon_off = {
        .uri = "/api/switch/forceon/off",
        .method = HTTP_POST,
        .handler = api_switch_forceon_off,
    };
    httpd_register_uri_handler(httpd, &forceon_off);

    httpd_uri_t forceoff_on = {
        .uri = "/api/switch/forceoff/on",
        .method = HTTP_POST,
        .handler = api_switch_forceoff_on,
    };
    httpd_register_uri_handler(httpd, &forceoff_on);

    httpd_uri_t forceoff_off = {
        .uri = "/api/switch/forceoff/off",
        .method = HTTP_POST,
        .handler = api_switch_forceoff_off,
    };
    httpd_register_uri_handler(httpd, &forceoff_off);
}
