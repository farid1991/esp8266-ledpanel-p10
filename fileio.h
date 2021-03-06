#ifndef FILEIO_H
#define FILEIO_H

#include <ArduinoJson.h>
#include <LittleFS.h>

#define MSG_LEN 64
#define SSID_LEN 32
#define PASS_LEN 32
#define IP_MAX_LEN 16
#define DEFAULT_BRIGHTNESS 1024

#define CONFIG_IP "/configs/ip.json"
#define CONFIG_LED "/configs/led.json"
#define CONFIG_WIFI "/configs/wifi.json"

#define default_ssid "esp8266"
#define default_pass "12345678"

#define default_ip "192.168.1.1"
#define default_gateway "192.168.1.1"
#define default_subnet "255.255.255.0"

#define PRINT_SERIAL_LINE Serial.println(F("------------------------------------------------"))

enum {
    INTEGER_VALUE,
    FLOAT_VALUE,
    STRING_VALUE
};

typedef struct _Config {
    char ssid[SSID_LEN];
    char pass[PASS_LEN];
} Config;

typedef struct _IPConfig {
    char ip[IP_MAX_LEN];
    char gateway[IP_MAX_LEN];
    char subnet[IP_MAX_LEN];
} IPConfig;

typedef struct _LEDConfig {
    char led_msg[MSG_LEN];
    uint16_t led_bright;
} LEDConfig;

Config config;
IPConfig ipConfig;
LEDConfig ledConfig;

bool start_filesystem() {
    Serial.println(F("[INFO] Mount LittleFS"));
    if (LittleFS.begin()) {
        Serial.println(F("[INFO] LittleFS mounted"));
        return true;
    }
    Serial.println(F("[ERROR] Failed Mount LittleFS"));
    return false;
}

int file_size(const char* path) {
    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.println(F("[ERROR] Failed to open file for reading"));
        return 0;
    }

    int fsize = file.size();

    file.close();
    return fsize;
}

String read_file(const char* path) {
    PRINT_SERIAL_LINE;
    Serial.printf(PSTR("[INFO] Read file: %s\n"), path);

    String result = "";

    File file = LittleFS.open(path, "r");
    if (!file) {
        Serial.println(F("[ERROR] Failed to open file for reading"));
        PRINT_SERIAL_LINE;
        return result;
    }

    while (file.available()) {
        result += (char)file.read();
    }

    Serial.println(result);

    file.close();
    PRINT_SERIAL_LINE;
    return result;
}

bool write_file(const char* path, String message) {
    Serial.printf(PSTR("Write file: %s\n"), path);

    File file = LittleFS.open(path, "w");
    if (!file) {
        Serial.println(F("[ERROR] Failed to open file for writing"));
        return false;
    }

    if (file.print(message)) {
        Serial.println(F("File written"));
    } else {
        Serial.println(F("[ERROR] Failed for write to file"));
        file.close();
        return false;
    }
    file.close();
    return true;
}

void delete_file(const char* path) {
    PRINT_SERIAL_LINE;
    if (LittleFS.exists(path)) {
        LittleFS.remove(path);
        Serial.printf(PSTR("[LOG] File %s deleted\n"), path);
    }
    Serial.printf(PSTR("[LOG] File %s not exist\n"), path);
    PRINT_SERIAL_LINE;
}

void load_ip_config() {
    String json_buff = read_file(CONFIG_IP);

    StaticJsonDocument<256> json_doc;
    DeserializationError error = deserializeJson(json_doc, json_buff);
    if (error)
        Serial.println(F("[ERROR] Failed to read file, using default configuration"));

    // Copy values from the JsonDocument to the Config
    strlcpy(ipConfig.ip,                  // <- destination
            json_doc["ip"] | default_ip,  // <- source
            sizeof(ipConfig.ip));         // <- destination's capacity

    strlcpy(ipConfig.gateway,                       // <- destination
            json_doc["gateway"] | default_gateway,  // <- source
            sizeof(ipConfig.gateway));              // <- destination's capacity

    strlcpy(ipConfig.subnet,                      // <- destination
            json_doc["subnet"] | default_subnet,  // <- source
            sizeof(ipConfig.subnet));             // <- destination's capacity
}

void load_wifi_config() {
    String json_buff = read_file(CONFIG_WIFI);

    StaticJsonDocument<256> json_doc;
    DeserializationError error = deserializeJson(json_doc, json_buff);
    if (error)
        Serial.println(F("[ERROR] Failed to read file, using default configuration"));

    // Copy values from the JsonDocument to the Config
    strlcpy(config.ssid,                      // <- destination
            json_doc["ssid"] | default_ssid,  // <- source
            sizeof(config.ssid));             // <- destination's capacity

    strlcpy(config.pass,                      // <- destination
            json_doc["pass"] | default_pass,  // <- source
            sizeof(config.pass));             // <- destination's capacity
}

void load_led_config() {
    String json_buff = read_file(CONFIG_LED);

    StaticJsonDocument<256> json_doc;
    DeserializationError error = deserializeJson(json_doc, json_buff);
    if (error)
        Serial.println(F("[ERROR] Failed to read file, using default configuration"));

    // Copy values from the JsonDocument to the Config
    strlcpy(ledConfig.led_msg,                    // <- destination
            json_doc["message"] | "==ESP8266==",  // <- source
            sizeof(ledConfig.led_msg));           // <- destination's capacity

    ledConfig.led_bright = isnan(json_doc["brightness"]) ? DEFAULT_BRIGHTNESS : json_doc["brightness"];

    Serial.println("LED Brightness: " + String(ledConfig.led_bright));
}

void save_to_json(const char* path, String key, String value, char type) {
    PRINT_SERIAL_LINE;
    Serial.printf(PSTR("[INFO] Writing file: %s\n"), path);

    File config_file;
    config_file = LittleFS.open(path, "r");
    if (!config_file) {
        Serial.println(F("[ERROR] Failed to open file for writing"));
        return;
    }

    DynamicJsonDocument json_doc(2048);
    deserializeJson(json_doc, config_file);
    config_file.close();

    switch (type) {
        case INTEGER_VALUE:
            json_doc[key] = value.toInt();
            break;
        case FLOAT_VALUE:
            json_doc[key] = value.toFloat();
            break;
        case STRING_VALUE:
        default:
            json_doc[key] = value;
            break;
    }

    config_file = LittleFS.open(path, "w");
    serializeJsonPretty(json_doc, config_file);
    config_file.close();
    Serial.println(F("CONFIG saved"));
    PRINT_SERIAL_LINE;
}

void save_config(const char* path, String key, String value) {
    save_to_json(path, key, value, STRING_VALUE);
}

void save_config(const char* path, String key, int value) {
    save_to_json(path, key, String(value), INTEGER_VALUE);
}

void save_config(const char* path, String key, float value) {
    save_to_json(path, key, String(value), FLOAT_VALUE);
}

#endif
