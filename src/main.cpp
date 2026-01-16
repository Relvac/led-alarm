#include <Arduino.h>
#include "config.h"
#include "led_controller.h"
#include "web_server.h"
#include <SPIFFS.h>
#include "alarm.h"
#include <RTClib.h>  // Для реального времени


LEDController ledController;
RTC_DS3231 rtc;
AlarmManager alarmManager;

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

    // Инициализация RTC
    if (!rtc.begin()) {
        Serial.println("RTC не найден!");
    }
    
    // Загрузка будильников из памяти
    alarmManager.loadFromPreferences();
    
    // Пример: создание тестового будильника
    Alarm alarm;
    alarm.id = alarmManager.getNextId();
    alarm.enabled = true;
    alarm.hour = 7;
    alarm.minute = 30;
    alarm.daysOfWeek = MONDAY | TUESDAY | WEDNESDAY | THURSDAY | FRIDAY; // Пн-Пт
    alarm.targetBrightness = 50;
    alarm.fadeDuration = 900; // 15 минут
    alarm.oneTime = false;
    
    alarmManager.addAlarm(alarm);
    alarmManager.saveToPreferences();
}

void loop() {
    server.handleClient();
    ledController.update();  // Обновление эффектов

    // Получаем текущее время
    DateTime now = rtc.now();
    uint8_t weekday = now.dayOfTheWeek(); // 0=Вс, 1=Пн, ..., 6=Сб
    
    // Проверяем будильники каждую минуту
    static uint8_t lastMinute = 255;
    if (now.minute() != lastMinute) {
        lastMinute = now.minute();
        
        // Получаем будильники на текущее время
        auto activeAlarms = alarmManager.getAlarmsForTime(
            now.hour(), now.minute(), weekday);
        
        // Обрабатываем каждый сработавший будильник
        for (auto& alarm : activeAlarms) {
            if (alarm.skipNext) {
                alarm.skipNext = false;
                alarmManager.updateAlarm(alarm);
                continue;
            }
            
            // Запускаем эффект "рассвет"
            startSunriseEffect(alarm.targetBrightness, alarm.fadeDuration);
            
            // Если будильник одноразовый - отключаем
            if (alarm.oneTime) {
                alarm.enabled = false;
                alarmManager.updateAlarm(alarm);
            }
        }
        
        // Сохраняем изменения
        if (!activeAlarms.empty()) {
            alarmManager.saveToPreferences();
        }
    }
    
    // Другие задачи...
    delay(1000);
}

void startSunriseEffect(uint8_t targetBrightness, uint16_t durationSec) {
    // Плавное увеличение яркости
    Serial.printf("Будильник! Яркость: %d%%, Длительность: %d сек\n", targetBrightness, durationSec);
    
    
}