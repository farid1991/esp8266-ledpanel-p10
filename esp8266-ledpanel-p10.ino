#include "fileio.h"
#include "led.h"
#include "sensor.h"
#include "uptime.h"
#include "uwebserver.h"
#include "wifi.h"

void setup() {
    Serial.begin(115200);
    Serial.println();
    PRINT_SERIAL_LINE;
    Serial.println(F("[INFO] Serial begin!"));

    start_filesystem();

    load_ip_config();
    load_led_config();
    load_wifi_config();

    start_wifi();
    start_webserver();
    start_dht_sensor();
    start_dmd_led();

    PRINT_SERIAL_LINE;
}

void loop() {
    calc_uptime();
    read_dht_sensor();
    run_led();
}
