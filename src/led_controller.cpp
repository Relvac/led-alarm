#include "led_controller.h"
#include "config.h"

LEDController::LEDController() {}

void LEDController::begin() {
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(LED_PIN, PWM_CHANNEL);
    loadConfig();  // Загружаем настройки
    applyBrightness(settings.brightness);
}

void LEDController::applyBrightness(int percent) {
    percent = constrain(percent, 0, 100);
    int pwmValue = map(percent, 0, 100, 0, 255);
    ledcWrite(PWM_CHANNEL, pwmValue);
    settings.brightness = percent;
    saveConfig();
}

void LEDController::update() {
    if (settings.breathing) {
        // Эффект дыхания
        static unsigned long lastTime = 0;
        static int direction = 1;
        
        if (millis() - lastTime > settings.breathingSpeed / 200) {
            int newBrightness = settings.brightness + direction;
            
            if (newBrightness >= settings.maxBrightness) direction = -1;
            if (newBrightness <= settings.minBrightness) direction = 1;
            
            applyBrightness(newBrightness);
            lastTime = millis();
        }
    }
}