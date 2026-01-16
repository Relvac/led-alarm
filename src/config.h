#pragma once
#include <Arduino.h>

// Константы для управления
#define LED_PIN 16           // GPIO16 для ШИМ
#define PWM_CHANNEL 0        // Канал LEDC
#define PWM_FREQ 5000        // Частота 5 кГц (можно увеличить до 5000)
#define PWM_RESOLUTION 8     // Разрешение 8 бит (0-255)

// Структура настроек
struct Config {
    // Основные настройки
    int brightness = 50;        // 0-100%
    
    // Расписание
    bool scheduleEnabled = false;
    String onTime = "08:00";
    String offTime = "23:00";
};

extern Config settings;

void saveConfig();
void loadConfig();