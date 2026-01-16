#include "config.h"
#include <Preferences.h>

Preferences pref;
Config settings;

void saveConfig() {
    pref.begin("led-config", false);
    pref.putInt("brightness", settings.brightness);
    pref.putBool("schedule", settings.scheduleEnabled);
    pref.putString("onTime", settings.onTime);
    pref.putString("offTime", settings.offTime);
    pref.end();
}

void loadConfig() {
    pref.begin("led-config", true);
    settings.brightness = pref.getInt("brightness", 50);
    settings.scheduleEnabled = pref.getBool("schedule", false);
    settings.onTime = pref.getString("onTime", "08:00");
    settings.offTime = pref.getString("offTime", "23:00");
    pref.end();
}