#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

void setupOTA() {
    ArduinoOTA.onStart([](){
        Serial.printf("Started updating %s\n", ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "SPIFFS");
        if(ArduinoOTA.getCommand() == U_FS)
            SPIFFS.end();
    });

    ArduinoOTA.onEnd([](){
        Serial.println("Upload finished.");
    });
    
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total){
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });

    ArduinoOTA.onError([](ota_error_t error){
        const char* r[] = {"Auth Failed", "Begin Failed", "Connect Failed", "Receive Failed", "End Failed"};
        Serial.printf("Error[%u]: \n%s", error, r[error]);
    });

    ArduinoOTA.begin();
}