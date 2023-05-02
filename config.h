#include <ESP8266WiFi.h>
#include <Arduino.h>

extern StaticJsonDocument<1000> config;

IPAddress IPfromString(const char* src) {
    IPAddress t;
    t.fromString(src);
    return t;
}

void loadConfig(String filename) {
    File confFile = SPIFFS.open(filename, "r");
    if(!confFile)
        err("Can't open config file");
    DeserializationError deserr = deserializeJson(config, confFile.readString());
    if(!deserr) {
        for(const char* x : {"ssid", "pass", "staticIP", "dns", "gateway", "subnet", "secret", "dangerThreshold", "webhookURL", "webhookTemplate"})
            if(!config[x])
                err("Config file is missing one or more necessary fields");
    }
    else
        err("Can't parse config file");
}