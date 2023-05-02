#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

String HTTPget(String url) {
    String response = "";
    HTTPClient http;
    http.begin(url);
    http.setTimeout(10000);
    int code = http.GET();
    if(code >= 200 && code < 300)
        response = http.getString();
    else
        Serial.println((String)code + " " + http.errorToString(code) + " " + http.getString());
    http.end();
    return response;
}

String HTTPpost(String url, String data) {
    BearSSL::WiFiClientSecure client;
    client.setInsecure();
    String resp = "";
    HTTPClient http;
    http.begin(client, url);
    http.setTimeout(10000);
    http.addHeader("Content-Type", "application/json");
    int code = http.POST(data);
    if(code >= 200 && code < 300)
        resp = http.getString();
    else
        Serial.println((String)code + " " + http.errorToString(code) + " " + http.getString());
    http.end();
    return resp;
}