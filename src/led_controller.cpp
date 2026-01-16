#include "led_controller.h"
#include "config.h"

LEDController::LEDController() {}

void LEDController::begin() {
    if (isInitialized) return; // защита от повторной инициализации
    
    // Настройка ШИМ на канале LEDC
    ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
    ledcAttachPin(LED_PIN, PWM_CHANNEL);
    
    // Загружаем настройки
    loadConfig();
    
    // Применяем сохранённую яркость
    applyBrightness(settings.brightness);
    
    isInitialized = true;
    Serial.println("LED Controller initialized");
    Serial.print("PWM on GPIO"); Serial.print(LED_PIN);
    Serial.print(", Channel "); Serial.print(PWM_CHANNEL);
    Serial.print(", Freq "); Serial.print(PWM_FREQ); Serial.println("Hz");
}

void LEDController::applyBrightness(int percent) {
    // Ограничиваем диапазон
    percent = constrain(percent, 0, 100);
    _currentBrightness = percent;
    
    // Конвертируем проценты в значение ШИМ (0-255 для 8 бит)
    int pwmValue = map(percent, 0, 100, 0, 255);
    
    // Устанавливаем ШИМ
    ledcWrite(PWM_CHANNEL, pwmValue);
    
    // Обновляем настройки
    settings.brightness = percent;
    
    Serial.print("Brightness set to ");
    Serial.print(percent);
    Serial.print("% (PWM = ");
    Serial.print(pwmValue);
    Serial.println(")");
}