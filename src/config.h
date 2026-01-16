#pragma once
#include <Arduino.h>

// Структура настроек
struct Config {
    // LED Settings
    int Brightness = 50;        // 0-100%
    // Schedule
    bool ScheduleEnabled = false;
    String OnTime = "08:00";
    String OffTime = "23:00";
};

extern Config settings;

void saveConfig();
void loadConfig();