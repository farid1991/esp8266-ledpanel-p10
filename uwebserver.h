#ifndef UWEBSERVER_H
#define UWEBSERVER_H

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>

#include "fileio.h"
#include "mime.h"
#include "sensor.h"
#include "uptime.h"
#include "wifi.h"

AsyncWebServer server(80);

void log_access(AsyncWebServerRequest* request) {
    String path = request->url();
    String client_ip = request->client()->remoteIP().toString();
    String http_method = request->methodToString();

    PRINT_SERIAL_LINE;
    Serial.printf(PSTR("[LOG] %s %s %s at %d:%d:%d\n"), http_method.c_str(), path.c_str(), client_ip.c_str(), hours, minutes, seconds);
    PRINT_SERIAL_LINE;
}

void handle_file_notfound(AsyncWebServerRequest* request) {
    Serial.println(F("[ERROR] Path Not Found: 404"));
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, "/404.html", TEXT_HTML);
    request->send(response);
}

void handle_http_badmethod(AsyncWebServerRequest* request) {
    AsyncResponseStream* response = request->beginResponseStream(FPSTR(APP_JSON));
    response->printf("{ 'msg': 'HTTP_%s => Bad request method' }", request->methodToString());
    request->send(response);
}

void handle_get_from_fs(AsyncWebServerRequest* request) {
    String path = request->url();

    if (path.equals("/")) {
        path = path + F("index.html");
        Serial.printf("GET root: %s\n", path.c_str());
    }

    String path_GZipped = path + ".gz";

    if (LittleFS.exists(path_GZipped)) {
        String contentType = getContentType(path);
        AsyncWebServerResponse* response = request->beginResponse(LittleFS, path_GZipped, contentType);
        response->addHeader("Content-Encoding", "gzip");
        request->send(response);

        Serial.println("[INFO] contentType: " + contentType);
        Serial.printf("[INFO] Send response: Compressed %dB\n", file_size(path_GZipped.c_str()));

    } else if (LittleFS.exists(path)) {
        if (path.endsWith(".gz")) {
            AsyncWebServerResponse* response = request->beginResponse(LittleFS, path, FPSTR(TEXT_PLAIN));
            response->addHeader("Content-Encoding", "gzip");
            request->send(response);
            Serial.printf("[INFO] Send response: Compressed %dB\n", file_size(path.c_str()));
        } else {
            String contentType = getContentType(path);
            AsyncWebServerResponse* response = request->beginResponse(LittleFS, path, contentType);
            request->send(response);

            Serial.println("[INFO] contentType: " + contentType);
            Serial.printf("[INFO] Send response: Uncompressed %dB\n", file_size(path.c_str()));
        }
    } else {
        handle_file_notfound(request);
    }
}

void handle_get_sensorpage(AsyncWebServerRequest* request) {
    log_access(request);
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, "/sensor.html", FPSTR(TEXT_HTML));
    request->send(response);
}

void handle_get_sensordata(AsyncWebServerRequest* request) {
    log_access(request);
    String response = "";
    StaticJsonDocument<256> json_doc;
    json_doc["temperature"] = temp;
    json_doc["humidity"] = humd;
    json_doc["heatindex"] = heatIndex;
    serializeJsonPretty(json_doc, response);
    request->send_P(200, FPSTR(APP_JSON), response.c_str());
}

String get_dir_list(const char* path) {
    String json_str = "";

    DynamicJsonDocument file_doc(2048);
    JsonArray file_array = file_doc.to<JsonArray>();

    Dir root = LittleFS.openDir(path);

    while (root.next()) {
        File file = root.openFile("r");
        String name = root.fileName();
        size_t size = file.size();
        time_t created = file.getCreationTime();
        time_t modified = file.getLastWrite();

        struct tm* tmstruct = localtime(&created);
        char buff_created[1024];
        sprintf(buff_created, "%d/%02d/%02d %02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min);
        tmstruct = localtime(&modified);
        char buff_modified[1024];
        sprintf(buff_modified, "%d/%02d/%02d %02d:%02d", (tmstruct->tm_year) + 1900, (tmstruct->tm_mon) + 1, tmstruct->tm_mday, tmstruct->tm_hour, tmstruct->tm_min);

        JsonObject file_obj = file_doc.createNestedObject();
        file_obj["name"] = name;
        file_obj["size"] = size;
        file_obj["created"] = buff_created;
        file_obj["modified"] = buff_modified;
        file_obj["isdir"] = file.isDirectory() ? 1 : 0;

        file.close();
    }
    serializeJsonPretty(file_doc, json_str);

    if (json_str.equals("[]")) {
        json_str.clear();
        StaticJsonDocument<256> json_doc;
        json_doc["status"] = "error";
        json_doc["message"] = "No File founds";
        serializeJsonPretty(json_doc, json_str);
    }
    return json_str;
}

void handle_get_fs_data(AsyncWebServerRequest* request) {
    log_access(request);

    String currentPath = "/";

    if (request->hasParam("path")) {
        AsyncWebParameter* p = request->getParam("path");
        Serial.println("Name: " + p->name() + " Value: " + p->value());
        currentPath = p->value();
    }

    String response = get_dir_list(currentPath.c_str());
    request->send_P(200, FPSTR(APP_JSON), response.c_str());
}

void handle_get_filemanager(AsyncWebServerRequest* request) {
    log_access(request);
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, "/fm.html", FPSTR(TEXT_HTML));
    request->send(response);
}

void webserver_get_led(AsyncWebServerRequest* request) {
    log_access(request);
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, CONFIG_LED, FPSTR(APP_JSON));
    request->send(response);
}

void webserver_post_led(AsyncWebServerRequest* request, uint8_t* data) {
    log_access(request);

    String response;
    StaticJsonDocument<256> json_resp;
    StaticJsonDocument<512> json_data;

    DeserializationError error = deserializeJson(json_data, data);
    if (error) {
        Serial.printf("[ERROR]: %d\n", error);
        json_resp[F("error")] = true;
        json_resp["message"] = F("'Could not parse JSON");
        serializeJsonPretty(json_resp, response);
        request->send_P(200, FPSTR(APP_JSON), response.c_str());
        return;
    }

    if (json_data.containsKey("message")) {
        String new_message = json_data["message"];
        if (!new_message.length()) {
            json_resp["status"] = F("error");
            json_resp["message"] = F("Message can not be blank");
        } else if (new_message.length() < MSG_LEN) {
            save_config(CONFIG_LED, "message", new_message);
            load_led_config();

            json_resp["status"] = F("done");
            json_resp["message"] = F("Message received, JSON saved");
        } else {
            json_resp["status"] = F("error");
            json_resp["message"] = F("Message too long(>64), JSON not saved");
        }
    } else if (json_data.containsKey("brightness")) {
        int new_brightness = json_data["brightness"];

        save_config(CONFIG_LED, "brightness", new_brightness);
        load_led_config();

        json_resp["status"] = F("done");
        json_resp["message"] = F("Message received, JSON saved");
    } else {
        json_resp["status"] = F("error");
        json_resp["message"] = F("invalid key, JSON not saved'");
    }
    serializeJsonPretty(json_resp, response);
    request->send_P(200, FPSTR(APP_JSON), response.c_str());
}

void handle_get_sysinfo(AsyncWebServerRequest* request) {
    log_access(request);

    String response;
    StaticJsonDocument<256> json_doc;

    json_doc["CpuFreqMHz"] = ESP.getCpuFreqMHz();
    json_doc["ChipId"] = ESP.getChipId();
    json_doc["FreeHeap"] = ESP.getFreeHeap();
    json_doc["FlashChipId"] = ESP.getFlashChipId();
    json_doc["FlashChipSize"] = ESP.getFlashChipSize();
    json_doc["FlashChipRealSize"] = ESP.getFlashChipRealSize();
    json_doc["SdkVersion"] = ESP.getSdkVersion();

    serializeJsonPretty(json_doc, response);
    request->send_P(200, FPSTR(APP_JSON), response.c_str());
}

void handle_get_wifistatus(AsyncWebServerRequest* request) {
    log_access(request);

    String response = "";
    StaticJsonDocument<512> json_doc;

    WiFiMode_t w_mode = WiFi.getMode();
    json_doc["Mode"] = WIFI_AP == w_mode ? F("WIFI_AP") : WIFI_AP_STA == w_mode ? F("WIFI_AP_STA")
                                                      : WIFI_STA == w_mode      ? F("WIFI_STA")
                                                      : WIFI_OFF == w_mode      ? F("WIFI_OFF")
                                                                                : F("UNKNOWN");

    json_doc["softAPIP"] = WiFi.softAPIP().toString();
    json_doc["localIP"] = WiFi.localIP().toString();
    json_doc["gatewayIP"] = WiFi.gatewayIP().toString();
    json_doc["subnetMask"] = WiFi.subnetMask().toString();
    json_doc["dnsIP"] = WiFi.dnsIP().toString();
    json_doc["hostname"] = WiFi.hostname();
    json_doc["macAddress"] = WiFi.macAddress();
    json_doc["SSID"] = WiFi.SSID();
    json_doc["BSSID"] = WiFi.BSSIDstr();
    json_doc["RSSI"] = WiFi.RSSI();

    serializeJsonPretty(json_doc, response);
    request->send_P(200, FPSTR(APP_JSON), response.c_str());
}

void handle_get_uptime(AsyncWebServerRequest* request) {
    log_access(request);

    String response;
    StaticJsonDocument<256> json_doc;
    json_doc["d"] = days;
    json_doc["h"] = hours;
    json_doc["m"] = minutes;
    json_doc["s"] = seconds;

    serializeJsonPretty(json_doc, response);
    request->send_P(200, FPSTR(APP_JSON), response.c_str());
}

void handle_notfound(AsyncWebServerRequest* request) {
    log_access(request);
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, "/404.html", FPSTR(TEXT_HTML));
    request->send(response);
}

void webserver_get_wifi(AsyncWebServerRequest* request) {
    log_access(request);
    AsyncWebServerResponse* response = request->beginResponse(LittleFS, CONFIG_WIFI, FPSTR(APP_JSON));
    request->send(response);
}

void webserver_post_wifi(AsyncWebServerRequest* request, uint8_t* data) {
    log_access(request);

    String response;
    StaticJsonDocument<256> json_resp;
    StaticJsonDocument<512> json_data;
    DeserializationError error = deserializeJson(json_data, data);
    if (error) {
        Serial.printf("[ERROR]: %d\n", error);
        json_resp["status"] = F("error");
        json_resp["message"] = F("Could not parse JSON");
        serializeJsonPretty(json_resp, response);
        request->send_P(200, FPSTR(APP_JSON), response.c_str());
        return;
    }

    if (json_data.containsKey("ssid") && json_data.containsKey("pass")) {
        String new_ssid = json_data["ssid"];
        String new_pass = json_data["pass"];

        if (!new_ssid.length() && !new_pass.length()) {
            json_resp["status"] = F("error");
            json_resp["message"] = "SSID & Password can not be blank";
        } else if (!new_ssid.length()) {
            json_resp["status"] = F("error");
            json_resp["message"] = "SSID can not be blank";
        } else if (!new_pass.length()) {
            json_resp["status"] = F("error");
            json_resp["message"] = "Password can not be blank";
        } else if (new_ssid.length() > 32 && new_pass.length() > 32) {
            json_resp["status"] = F("error");
            json_resp["message"] = "SSID & Password too long";
        } else if (new_ssid.length() > 32) {
            json_resp["status"] = F("error");
            json_resp["message"] = F("SSID too long");
        } else if (new_pass.length() > 32) {
            json_resp["status"] = F("error");
            json_resp["message"] = F("Password too long");
        } else {
            json_resp["status"] = F("done");
            json_resp["message"] = F("New SSID & Password saved");
            Serial.println(new_ssid + " " + new_pass);
            save_config(CONFIG_WIFI, "ssid", new_ssid);
            save_config(CONFIG_WIFI, "pass", new_pass);
            load_wifi_config();
            restart_wifi();
        }
    } else {
        json_resp["status"] = F("error");
        json_resp["message"] = F("invalid key, JSON not saved'");
    }
    serializeJsonPretty(json_resp, response);
    request->send_P(200, FPSTR(APP_JSON), response.c_str());
}

void handle_get_request(AsyncWebServerRequest* request) {
    log_access(request);

    if (request->method() == HTTP_GET) {
        handle_get_from_fs(request);
    } else {
        handle_http_badmethod(request);
    }
}

void handle_post_request(AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
    log_access(request);

    String path = request->url();

    if (request->method() == HTTP_POST) {
        if (path.equals("/led")) {
            webserver_post_led(request, data);
        } else if (path.equals("/wifi")) {
            webserver_post_wifi(request, data);
        } else {
            Serial.printf("\n [DATA] %s\n", data);
        }
    }
}

void server_routing() {
    server.on("/404", HTTP_GET, handle_notfound);
    server.on("/dht", HTTP_GET, handle_get_sensordata);
    server.on("/sensor", HTTP_GET, handle_get_sensorpage);
    server.on("/fm", HTTP_GET, handle_get_filemanager);
    server.on("/fs", HTTP_GET, handle_get_fs_data);
    server.on("/system", HTTP_GET, handle_get_sysinfo);
    server.on("/uptime", HTTP_GET, handle_get_uptime);
    server.on("/status", HTTP_GET, handle_get_wifistatus);
    server.on("/led", HTTP_GET, webserver_get_led);
    server.on("/wifi", HTTP_GET, webserver_get_wifi);

    server.onNotFound(handle_get_request);
    server.onRequestBody(handle_post_request);
}

void start_webserver() {
    server_routing();
    server.begin();
}

#endif
