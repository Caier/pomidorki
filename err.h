#include <Arduino.h>

void err(String msg) {
    while(1) {
        for(int i = 0; i < 5; i++) {
            Serial.println("ERROR: " + msg);
            delay(500);
        }
        ESP.restart();
    }
}