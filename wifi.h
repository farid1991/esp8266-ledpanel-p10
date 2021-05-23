#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>

#include "fileio.h"

IPAddress local_ip;
IPAddress gateway;
IPAddress subnet;

void print_WiFi() {
    Serial.println(F("[INFO] Access Point started"));
    Serial.print(F("[INFO] IP Address: "));
    Serial.println(WiFi.softAPIP());
}

void reload_wifi_config() {
    local_ip.fromString(ipConfig.ip);
    gateway.fromString(ipConfig.gateway);
    subnet.fromString(ipConfig.subnet);
}

void restart_wifi() {
    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_AP);

    reload_wifi_config();
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(config.ssid, config.pass);

    print_WiFi();
}

void start_wifi() {
    WiFi.mode(WIFI_AP);
    reload_wifi_config();

    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(config.ssid, config.pass);

    print_WiFi();
}

#endif