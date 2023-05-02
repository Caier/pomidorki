#include "peripherals.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <TimeLib.h>
#include "ota.h"
#include "http.h"
#include "err.h"
#include "config.h"
StaticJsonDocument<1000> config;
#include "frameTasks.h"
#include "routes.h"

#define configFile "/config.json"
#define CC const char*
#define ip(x) IPfromString(config[x])

AsyncWebServer server(80);

void setup() {
    Serial.begin(115200);
    setupPins();
    digitalWrite(yLed, 0);
    WiFi.mode(WIFI_STA);

    if(!SPIFFS.begin())
        err("An Error has occurred while mounting SPIFFS"); // err.h
    if(!SPIFFS.exists(configFile))
        err("Can't find config file");
    else 
        loadConfig(configFile); // config.h
    
    Serial.print("\nConnecting with: " + (String)(CC)config["ssid"]);
    WiFi.config(ip("staticIP"), ip("dns"), ip("gateway"), ip("subnet") );
    WiFi.begin((CC)config["ssid"], (CC)config["pass"]);
    Serial.println();
    while(WiFi.status() != WL_CONNECTED) {
        wl_status_t status = WiFi.status();
        if(status == WL_NO_SSID_AVAIL || status == WL_CONNECT_FAILED)
            err("Could not connect to the provided WiFi network");
        Serial.print(".");
        if(millis() > 10000)
            ESP.restart();
        delay(300);
    }

    Serial.println((String)"\nWiFi connected.\nIP address: " + WiFi.localIP().toString());

    setupOTA();
    setRoutes(&server); // routes.h
    
    String timeres = HTTPget("http://worldtimeapi.org/api/ip");
    if(timeres.length() > 0) {
        StaticJsonDocument<650> timeinfo;
        if(deserializeJson(timeinfo, timeres).code() != OK)
            err("Can't deserialize time json");
        setTime((time_t)timeinfo["unixtime"]);
        String ts = timeinfo["utc_offset"];
        adjustTime((ts[0] == '+' ? 1 : -1) * (ts.substring(1, 3).toInt() * 3600 + ts.substring(4, 6).toInt() * 60));
    }
    else
        err("Can't get time");

    server.begin();
    digitalWrite(yLed, 1);
    Serial.println("Setup done.");
}

void loop() {
    for(auto t : tasks)
        t->render();
    ArduinoOTA.handle();
}