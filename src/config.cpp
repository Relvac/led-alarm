#include "config.h"
#include <Preferences.h>

Preferences pref;
Config settings;

void saveConfig() {
    pref.begin("led-config", false);
    
    pref.putInt("Brightness", settings.Brightness);
    pref.putInt("AwakeningTime", settings.AwakeningTime);
    pref.putBool("Schedule", settings.ScheduleEnabled);
    pref.putString("OnTime", settings.OnTime);
    pref.putString("OffTime", settings.OffTime);
    
    pref.end();
}

void loadConfig() {
    pref.begin("led-config", true);
    
    settings.brightness = pref.getInt("Brightness", 50);
    settings.scheduleEnabled = pref.getBool("Schedule", false);
    settings.onTime = pref.getString("OnTime", "08:00");
    settings.offTime = pref.getString("OffTime", "23:00");
    settings.AwakeningTime = pref.getInt("AwakeningTime",30); // 30 is default value 
    
    pref.end();
}