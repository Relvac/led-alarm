const int pwmPin = 16; // GPIO16 for LED
const int freq = 5000; // frequency  5 KHz
const int resolution = 8; // 8 bit 

void setup() {
    // pwm settings 
    ledcSetup(0, freq, resolution);
    ledcAttachPin(pwmPin, pwmChannel);
    
}

void loop() {
    for (int i = 0; i <= 256; i++) {
        ledcWrite(pwmChannel, i);
        delay(1);
    }
}