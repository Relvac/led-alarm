#include <Arduino.h>
#include "config.h"
#include "led_controller.h"
#include "web_server.h"
#include <SPIFFS.h>

LEDController ledController;

void setup() {
    Serial.begin(115200);
    
    // Инициализация SPIFFS для веб-страниц
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS ошибка!");
        return;
    }
    
    // Загрузка настроек
    loadConfig();
    
    // Инициализация контроллера
    ledController.begin();
    
    // Запуск веб-сервера
    setupWebServer();
    
    Serial.println("Система запущена");
}

void loop() {
    server.handleClient();
    ledController.update();  // Обновление эффектов
}