#ifndef SENSOR_H
#define SENSOR_H

#include <DHT.h>

#include "pins.h"
#include "uptime.h"

double temp, humd, heatIndex;

DHT dht(PIN_DHT, DHT11);

void start_dht_sensor() {
    dht.begin();
}

void read_dht_sensor() {
    if (time_elapsed > (TIME_INTERVAL * 2)) {
        temp = dht.readTemperature();
        humd = dht.readHumidity();
        heatIndex = dht.computeHeatIndex(false);
        time_elapsed = 0;
    }
}

#endif