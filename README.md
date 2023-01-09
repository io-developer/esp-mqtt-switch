# ESP MQTT Switch
Configurable firmware for IoT switch based on ESP8266 chip (and maybe ESP32). Using Espressif SDK and MQTT protocol for messaging.

### Install
* Prepare toolchain following the official guide - https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/linux-setup.html
* Cd to project dir
* Run `make all`
* Plug in your ESP chip to serial port and flash `make flash`
* Run `make monitor` (turn off/on) to watch live log

### Config
* To put switch to configuration mode - quick turn off/on chip 3+ times. You should see slowly blinking builtin LED for 1 sec every second.
* Discover WiFi. Find 'esp-XXXXXX' access point and connect with password `esp_admin` (`/main/app_wifi.h  APP_WIFI_DEFAULT_AP_PASS`)
* To request current WiFi config:
```
curl -X GET http://192.168.4.1/api/wifi/config
```
* To update WiFi config. Example:
```
curl -X POST http://192.168.4.1/api/wifi/config --data '{"ssid": "yourssid", "password": "yourpass"}'
```
* To request current MQTT config:
```
curl -X GET http://192.168.4.1/api/mqtt/config
```
* To update MQTT config. Example:
```
curl -X POST http://192.168.4.1/api/mqtt/config --data '{"uri": "mqtt://your.mqtt.host",  "username": "mqtt", "password": "yourpass",  "topic_forceon": {"path": "home/ups/state/onbattery"}}'
```
* Restart:
```
curl -X POST http://192.168.4.1/api/restart
```

### API
Discover api endpoints & sources for more details here: /main/app_api*.c