#ifndef LED_H
#define LED_H

#include <DMDESP.h>
#include <fonts/DejaVuSans9.h>
#include <fonts/Droid_Sans_12.h>
#include <fonts/Droid_Sans_16.h>
#include <fonts/ElektronMart6x8.h>
#include <fonts/Mono5x7.h>
#include <fonts/SystemFont5x7.h>

#include "fileio.h"
#include "pins.h"
#include "sensor.h"
#include "uptime.h"

// Set Width and Hight to the number of displays you have
#define WIDTH 1
#define HEIGHT 1

DMDESP Disp(WIDTH, HEIGHT);

String Sensor_text;
char stext[64];

static char* teks[] = {ledConfig.led_msg};

void runningTexts(int yPos, int width, uint8_t speed) {
    static uint32_t pM;
    static uint32_t xPos;
    Disp.setFont(Droid_Sans_16);
    int fullScroll = Disp.textWidth(teks[0]) + width;
    if ((millis() - pM) > speed) {
        pM = millis();
        if (xPos < fullScroll) {
            ++xPos;
        } else {
            xPos = 0;
            return;
        }
        Disp.drawText(width - xPos, yPos, teks[0]);
    }
}

void start_dmd_led() {
    Disp.start();                              // Start DMDESP
    Disp.setBrightness(ledConfig.led_bright);  // Set Brightness
    Disp.setFont(Droid_Sans_16);               // Set Font
}

void run_led() {
    Disp.loop();

    if (ledeElapsed > (2 * TIME_INTERVAL)) {
        Sensor_text = "Suhu: " + String(temp) + "C " + String(humd) + "%";
        strcpy(stext, Sensor_text.c_str());

        ledeElapsed = 0;
    }

    //Disp.drawText(0, 0, ledconfig.led_msg);  // Show texts
    runningTexts(0, Disp.width(), 40);  // Show running texts
}

#endif