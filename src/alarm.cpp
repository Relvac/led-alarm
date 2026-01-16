#include "alarm.h"
#include <Preferences.h>
#include <ArduinoJson.h>

// Константы
#define ALARM_NAMESPACE "alarms"
#define MAX_ALARMS 10  // Максимум будильников (можно увеличить)

// Флаги дней недели
enum Weekdays {
    MONDAY = 0x01,    // 00000001
    TUESDAY = 0x02,   // 00000010
    WEDNESDAY = 0x04, // 00000100
    THURSDAY = 0x08,  // 00001000
    FRIDAY = 0x10,    // 00010000
    SATURDAY = 0x20,  // 00100000
    SUNDAY = 0x40     // 01000000
};

// ==================== КЛАСС Alarm ====================

bool Alarm::matchesTime(uint8_t h, uint8_t m, uint8_t weekday) {
    if (!enabled) return false;
    if (hour != h || minute != m) return false;
    return isDayActive(weekday);
}

bool Alarm::isDayActive(uint8_t weekday) {
    // weekday: 0=Пн, 1=Вт, ..., 6=Вс
    uint8_t dayMask = 1 << weekday;
    return (daysOfWeek & dayMask) != 0;
}

String Alarm::toJSON() {
    StaticJsonDocument<256> doc;
    
    doc["id"] = id;
    doc["enabled"] = enabled;
    doc["hour"] = hour;
    doc["minute"] = minute;
    doc["days"] = daysOfWeek;
    doc["brightness"] = targetBrightness;
    doc["duration"] = fadeDuration;
    doc["oneTime"] = oneTime;
    doc["skipNext"] = skipNext;
    
    String json;
    serializeJson(doc, json);
    return json;
}

Alarm Alarm::fromJSON(const String& json) {
    StaticJsonDocument<256> doc;
    deserializeJson(doc, json);
    
    Alarm alarm;
    alarm.id = doc["id"] | 0;
    alarm.enabled = doc["enabled"] | true;
    alarm.hour = doc["hour"] | 7;
    alarm.minute = doc["minute"] | 0;
    alarm.daysOfWeek = doc["days"] | 0x7F; // Все дни
    alarm.targetBrightness = doc["brightness"] | 50;
    alarm.fadeDuration = doc["duration"] | 30; // 30 минут
    alarm.oneTime = doc["oneTime"] | false;
    alarm.skipNext = doc["skipNext"] | false;
    
    return alarm;
}

// ==================== КЛАСС AlarmManager ====================

bool AlarmManager::addAlarm(const Alarm& alarm) {
    if (alarms.size() >= MAX_ALARMS) return false;
    
    // Проверяем дубликаты по времени
    for (const auto& a : alarms) {
        if (a.hour == alarm.hour && 
            a.minute == alarm.minute && 
            a.daysOfWeek == alarm.daysOfWeek) {
            return false; // Будильник на это время уже есть
        }
    }
    
    alarms.push_back(alarm);
    if (alarm.id >= nextId) {
        nextId = alarm.id + 1;
    }
    return true;
}

bool AlarmManager::removeAlarm(uint8_t id) {
    for (auto it = alarms.begin(); it != alarms.end(); ++it) {
        if (it->id == id) {
            alarms.erase(it);
            return true;
        }
    }
    return false;
}

bool AlarmManager::updateAlarm(const Alarm& alarm) {
    for (auto& a : alarms) {
        if (a.id == alarm.id) {
            a = alarm;
            return true;
        }
    }
    return false;
}

Alarm* AlarmManager::getAlarm(uint8_t id) {
    for (auto& alarm : alarms) {
        if (alarm.id == id) return &alarm;
    }
    return nullptr;
}

std::vector<Alarm> AlarmManager::getAllAlarms() {
    return alarms;
}

std::vector<Alarm> AlarmManager::getEnabledAlarms() {
    std::vector<Alarm> enabled;
    for (const auto& alarm : alarms) {
        if (alarm.enabled) enabled.push_back(alarm);
    }
    return enabled;
}

std::vector<Alarm> AlarmManager::getAlarmsForTime(uint8_t hour, uint8_t minute, uint8_t weekday) {
    std::vector<Alarm> matching;
    for (const auto& alarm : alarms) {
        if (alarm.matchesTime(hour, minute, weekday)) {
            matching.push_back(alarm);
        }
    }
    return matching;
}

bool AlarmManager::saveToPreferences() {
    Preferences prefs;
    prefs.begin(ALARM_NAMESPACE, false);
    
    // Сохраняем количество будильников
    prefs.putUInt("count", alarms.size());
    prefs.putUInt("nextId", nextId);
    
    // Сохраняем каждый будильник как JSON
    for (size_t i = 0; i < alarms.size(); i++) {
        String key = "alarm_" + String(i);
        String json = alarms[i].toJSON();
        prefs.putString(key.c_str(), json);
    }
    
    // Сохраняем как JSON массив (альтернативный способ)
    String jsonArray = toJSONArray();
    prefs.putString("json_array", jsonArray);
    
    prefs.end();
    return true;
}

bool AlarmManager::loadFromPreferences() {
    Preferences prefs;
    prefs.begin(ALARM_NAMESPACE, true);
    
    alarms.clear();
    
    // Способ 1: Загружаем из JSON массива
    String jsonArray = prefs.getString("json_array", "[]");
    if (jsonArray != "[]") {
        fromJSONArray(jsonArray);
        nextId = prefs.getUInt("nextId", 1);
        prefs.end();
        return true;
    }
    
    // Способ 2: Загружаем по одному (старый формат)
    size_t count = prefs.getUInt("count", 0);
    nextId = prefs.getUInt("nextId", 1);
    
    for (size_t i = 0; i < count; i++) {
        String key = "alarm_" + String(i);
        String json = prefs.getString(key.c_str(), "");
        if (json != "") {
            Alarm alarm = Alarm::fromJSON(json);
            alarms.push_back(alarm);
        }
    }
    
    prefs.end();
    return true;
}

void AlarmManager::clearAll() {
    alarms.clear();
    nextId = 1;
    
    Preferences prefs;
    prefs.begin(ALARM_NAMESPACE, false);
    prefs.clear();
    prefs.end();
}

size_t AlarmManager::count() {
    return alarms.size();
}

size_t AlarmManager::memoryUsage() {
    // Приблизительный расчёт памяти
    size_t alarmSize = sizeof(Alarm); // ~12 байт
    size_t vectorOverhead = sizeof(std::vector<Alarm>); // ~24 байта
    return (alarms.capacity() * alarmSize) + vectorOverhead;
}

uint8_t AlarmManager::getNextId() {
    return nextId++;
}

String AlarmManager::toJSONArray() {
    StaticJsonDocument<4096> doc; // До 4КБ для будильников
    JsonArray array = doc.to<JsonArray>();
    
    for (const auto& alarm : alarms) {
        JsonObject obj = array.createNestedObject();
        obj["id"] = alarm.id;
        obj["enabled"] = alarm.enabled;
        obj["hour"] = alarm.hour;
        obj["minute"] = alarm.minute;
        obj["days"] = alarm.daysOfWeek;
        obj["brightness"] = alarm.targetBrightness;
        obj["duration"] = alarm.fadeDuration;
        obj["oneTime"] = alarm.oneTime;
        obj["skipNext"] = alarm.skipNext;
    }
    
    String json;
    serializeJson(doc, json);
    return json;
}

bool AlarmManager::fromJSONArray(const String& json) {
    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, json);
    
    if (error) return false;
    
    alarms.clear();
    nextId = 1;
    
    JsonArray array = doc.as<JsonArray>();
    for (JsonObject obj : array) {
        Alarm alarm;
        alarm.id = obj["id"] | 0;
        alarm.enabled = obj["enabled"] | true;
        alarm.hour = obj["hour"] | 7;
        alarm.minute = obj["minute"] | 0;
        alarm.daysOfWeek = obj["days"] | 0x7F;
        alarm.targetBrightness = obj["brightness"] | 50;
        alarm.fadeDuration = obj["duration"] | 30;
        alarm.oneTime = obj["oneTime"] | false;
        alarm.skipNext = obj["skipNext"] | false;
        
        alarms.push_back(alarm);
        if (alarm.id >= nextId) {
            nextId = alarm.id + 1;
        }
    }
    
    return true;
}