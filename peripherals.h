#include "pins/pins_arduino.h"
#include <Arduino.h>

#ifndef peripherals
#define peripherals

#define button D0
#define pumps D1
#define analA D2
#define analB D3
#define ultraTrig D4
#define ultraEcho D5
#define meterVcc D6
#define yLed D7
#define analCom A0
#define meter1 0
#define meter2 1
#define waterVolume 2

bool TT[4][2] = {{0,0}, {1,0}, {0,1}, {1,1}};

void setupPins() {
    pinMode(analCom, INPUT);
    pinMode(button, INPUT_PULLDOWN_16);
    pinMode(ultraEcho, INPUT);
    for(auto i : (uint8_t[]){D1, D2, D3, D4, D6, D7})
        pinMode(i, OUTPUT);
}

void selectAnal(int pin) {
    digitalWrite(analA, TT[pin][0]);
    digitalWrite(analB, TT[pin][1]);
}

int analRead(int pin) {
    selectAnal(pin);
    return analogRead(analCom);
}

void analWrite(int pin, unsigned int val) {
    selectAnal(pin);
    analogWrite(analCom, val);
}

float ultraMeasure() {
    digitalWrite(ultraTrig, 0);
    delayMicroseconds(2);
    digitalWrite(ultraTrig, 1);
    delayMicroseconds(10);
    digitalWrite(ultraTrig, 0);
    return (pulseIn(ultraEcho, 1) / 2) * 0.0343;
}

struct { 
    int met1 = analRead(meter1),
        met2 = analRead(meter2),
        pumpLength = analRead(waterVolume) * 107.42 + 10000;
    float waterSurface = ultraMeasure();
} Measurements;

#endif