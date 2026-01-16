#pragma once
#include <Arduino.h>

class LEDController {
private:
    bool isInitialized = false;
    
public:
    LEDController();
    void begin();
    void applyBrightness(int percent);  // 0-100%
    int getCurrentBrightness() { return _currentBrightness; }
    
private:
    int _currentBrightness = 0;
};