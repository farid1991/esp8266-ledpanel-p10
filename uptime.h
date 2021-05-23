#ifndef UPTIME_H
#define UPTIME_H

#include <elapsedMillis.h>

#define TIME_INTERVAL 1000

uint8_t seconds = 0;
uint8_t minutes = 0;
uint8_t hours = 0;
uint8_t days = 0;
uint8_t months = 0;

elapsedMillis time_elapsed;
elapsedMillis uptime;
elapsedMillis ledeElapsed;

void calc_uptime() {
    if (uptime > TIME_INTERVAL) {
        seconds++;
        if (seconds >= 60) {
            minutes++;
            seconds = 0;
        }
        if (minutes >= 60) {
            hours++;
            minutes = 0;
        }
        if (hours >= 24) {
            days++;
            hours = 0;
        }
        uptime = 0;
    }
}

#endif