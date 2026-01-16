#include <Arduino.h>
#include "led_controller.h"

LEDController ledController;

void setup() {
    Serial.begin(115200);
    delay(1000); // Даём время для инициализации Serial
    
    Serial.println("\n=== LED Strip Controller ===");
    Serial.println("Initializing...");
    
    // Инициализация контроллера LED
    ledController.begin();
    
    Serial.println("System ready!");
    Serial.println("Commands: 0-100 - set brightness");
    Serial.println("Press 't' for test sequence");
}

void loop() {
    // Тестирование ШИМ с разными значениями яркости
    if (Serial.available()) {
        char cmd = Serial.read();
        
        if (cmd == 't') {
            // Тестовая последовательность
            Serial.println("Starting test sequence...");
            
            for (int i = 0; i <= 100; i += 10) {
                ledController.applyBrightness(i);
                delay(500);
            }
            
            for (int i = 100; i >= 0; i -= 10) {
                ledController.applyBrightness(i);
                delay(500);
            }
            
            // Возвращаем к сохранённому значению
            ledController.applyBrightness(settings.brightness);
            
        } else if (isdigit(cmd)) {
            // Ручной ввод яркости через Serial
            String input = Serial.readStringUntil('\n');
            int value = input.toInt();
            
            if (value >= 0 && value <= 100) {
                ledController.applyBrightness(value);
                Serial.print("Manual brightness set to: ");
                Serial.println(value);
            }
        }
    }
    
    // Можно добавить плавные эффекты здесь
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 10000) { // Каждые 10 секунд
        // Пример: цикличное изменение яркости
        // static int demoBrightness = 0;
        // static int demoDirection = 1;
        // demoBrightness += demoDirection * 10;
        // if (demoBrightness >= 100 || demoBrightness <= 0) demoDirection *= -1;
        // ledController.applyBrightness(demoBrightness);
        
        lastUpdate = millis();
    }
}