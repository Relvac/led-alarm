#pragma once
#include <Arduino.h>
#include <vector>
#include <ArduinoJson.h>

// Структура одного будильника
struct Alarm {
    uint8_t id;                 // Уникальный ID
    bool enabled;              // Включен ли будильник
    uint8_t hour;              // Час (0-23)
    uint8_t minute;            // Минута (0-59)
    
    // Дни недели (битовая маска)
    // Бит 0 = Понедельник, бит 1 = Вторник, ..., бит 6 = Воскресенье
    uint8_t daysOfWeek;        
    
    // Параметры освещения
    uint8_t targetBrightness;  // Целевая яркость (0-100%)
    uint16_t fadeDuration;     //* Длительность нарастания (секунды)  поменять на минуты 
    
    // Повторение и пропуск
    //!bool oneTime;              // Одноразовый будильник
    //!bool skipNext;             // Пропустить следующий срабатывание
    
    // Методы
    bool matchesTime(uint8_t h, uint8_t m, uint8_t weekday);
    bool isDayActive(uint8_t weekday);
    String toJSON();
    static Alarm fromJSON(const String& json);
};

// Класс для управления будильниками
class AlarmManager {
private:
    std::vector<Alarm> alarms;
    uint8_t nextId = 1;
    
public:
    // Основные методы
    bool addAlarm(const Alarm& alarm);
    bool removeAlarm(uint8_t id);
    bool updateAlarm(const Alarm& alarm);
    Alarm* getAlarm(uint8_t id);
    
    // Получение списков
    std::vector<Alarm> getAllAlarms();
    std::vector<Alarm> getEnabledAlarms();
    std::vector<Alarm> getAlarmsForTime(uint8_t hour, uint8_t minute, uint8_t weekday);
    
    // Работа с памятью
    bool saveToPreferences();
    bool loadFromPreferences();
    void clearAll();
    
    // Утилиты
    size_t count();
    size_t memoryUsage();
    uint8_t getNextId();
    
    // JSON
    String toJSONArray();
    bool fromJSONArray(const String& json);
};