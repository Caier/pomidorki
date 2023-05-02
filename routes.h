#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>

#define reqSUCC 200, "application/json", "{\"success\": true}"
#define respErr(msg) (String)"{\"success\": false, \"error\": \"" + msg + "\"}"
#define reqErr(msg) req->send(400, "application/json", respErr(msg))
#define chkauthh req->authenticate(config["secret"], config["secret"])

void setRoutes(AsyncWebServer* server) {
    server->onNotFound([](AsyncWebServerRequest *req){
        if(SPIFFS.exists(req->url())) {
            if(chkauthh)
                req->send(SPIFFS, req->url(), String(), true);
            else
                req->requestAuthentication();
        }
        else
            req->send(404);
    });

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *req){
        if(chkauthh)
            req->send(SPIFFS, "/index.html");
        else
            req->requestAuthentication();
    });
    
    server->on("/fs", HTTP_POST, [](AsyncWebServerRequest* req){
        if(chkauthh)
            req->send(200, "text/plain", "Uploading...");
        else
            req->requestAuthentication();
    }, [](AsyncWebServerRequest* req, String filename, size_t index, uint8_t *data, size_t len, bool final){
        if(!index && chkauthh) {
            req->_tempFile = SPIFFS.open(filename.startsWith("/") ? filename : "/" + filename, "w");
        }
        if(req->_tempFile && chkauthh) {
            if(len)
                req->_tempFile.write(data, len);
            if(final) {
                req->_tempFile.close();
                req->redirect("/");
            }
        }
    });

    server->on("/listfs", HTTP_GET, [](AsyncWebServerRequest* req){
        if(chkauthh) {
            String res = "";
            Dir root = SPIFFS.openDir("/");
            while(root.next()) {
                res += root.fileName() + (String)";;";
            }
            req->send(200, "text/plain", res);
        }
        else
            req->requestAuthentication();
    });

    server->on("/reboot", HTTP_GET, [](AsyncWebServerRequest* req){
        if(chkauthh) {
            req->send(200);
            ESP.restart();
        }
        else
            req->requestAuthentication();
    });

    server->on("/state", HTTP_GET, [](AsyncWebServerRequest* req) {
        req->send(200, "application/json", "{\"success\": true, \"state\": { \"meter1\": " + (String)Measurements.met1 + " , \"meter2\": " + (String)Measurements.met2 + ", \"pumpLength\": " + (String)Measurements.pumpLength + ", \"waterSurface\": " + (String)Measurements.waterSurface + " }}");
    });

    server->on("/pump", HTTP_POST, [](AsyncWebServerRequest *req){
        reqErr("empty request");
    },
    NULL,
    [](AsyncWebServerRequest* req, uint8_t* data, size_t len, size_t index, size_t total){
        String json = std::string((const char*)data).substr(0, len).c_str();
        StaticJsonDocument<500> jsdoc;
        DeserializationError deserr = deserializeJson(jsdoc, json);
        if(!deserr) {
            if(!jsdoc["secret"]) {
                reqErr("Request object must contain a 'secret' property");
                return;
            }
            String secret = (const char*)jsdoc["secret"];
            if(secret == config["secret"]) {
                int length;
                if(!jsdoc["length"])
                    length = Measurements.pumpLength;
                else
                    length = (int)jsdoc["length"];
                if(length <= 130000) {
                    if(onlinePump.pump(length))
                        req->send(reqSUCC);
                    else
                        reqErr("Pumps already toggled");
                }
                else
                    reqErr("Length too long.");
            } 
            else
                reqErr("Invalid secret.");
        }
        else
            reqErr(deserr.c_str());
    });
}