#include "web_server.h"
#include "config.h"
#include <ArduinoJson.h>

WebServer server(80);
LEDController led;

String getSettingsJSON() {
    StaticJsonDocument<512> doc;
    
    doc["brightness"] = settings.brightness;
    doc["maxBrightness"] = settings.maxBrightness;
    doc["minBrightness"] = settings.minBrightness;
    doc["fadeSpeed"] = settings.fadeSpeed;
    doc["autoOff"] = settings.autoOff;
    doc["autoOffTime"] = settings.autoOffTime;
    doc["breathing"] = settings.breathing;
    doc["breathingSpeed"] = settings.breathingSpeed;
    doc["scheduleEnabled"] = settings.scheduleEnabled;
    doc["onTime"] = settings.onTime;
    doc["offTime"] = settings.offTime;
    
    String output;
    serializeJson(doc, output);
    return output;
}

void handleAPI() {
    if (server.method() == HTTP_GET) {
        server.send(200, "application/json", getSettingsJSON());
    }
    else if (server.method() == HTTP_POST) {
        StaticJsonDocument<512> doc;
        deserializeJson(doc, server.arg("plain"));
        
        // Обновляем настройки
        if (doc.containsKey("brightness")) {
            settings.brightness = doc["brightness"];
            led.applyBrightness(settings.brightness);
        }
        if (doc.containsKey("maxBrightness")) settings.maxBrightness = doc["maxBrightness"];
        if (doc.containsKey("breathing")) settings.breathing = doc["breathing"];
        // ... остальные настройки
        
        saveConfig();
        server.send(200, "application/json", "{\"status\":\"ok\"}");
    }
}

void setupWebServer() {
    // API endpoint
    server.on("/api/settings", HTTP_ANY, handleAPI);
    
    // Веб-страница из SPIFFS
    server.serveStatic("/", SPIFFS, "/index.html");
    server.serveStatic("/style.css", SPIFFS, "/style.css");
    
    server.begin();
}