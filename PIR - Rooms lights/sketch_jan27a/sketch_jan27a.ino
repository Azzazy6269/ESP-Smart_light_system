#define PIR_PIN 35      // PIR sensor output pin
#define LED_PIN 6      // LED PWM pin
#define TIMEOUT 3600000  // 1 hour in milliseconds

unsigned long motionTimestamp = 0;
bool motionDetected = false;

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (digitalRead(PIR_PIN) == HIGH) {
    motionDetected = true;
    motionTimestamp = millis();
    analogWrite(LED_PIN, 255);  // Turn on LED at full brightness
  }

  // If no motion for 1 hour
  if (motionDetected && (millis() - motionTimestamp >= TIMEOUT)) {
    gradualDim();
    motionDetected = false;  // Reset the flag
  }
}

// Gradually dims the light
void gradualDim() {
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    analogWrite(LED_PIN, brightness);
    delay(5000);  // Delay to allow human detection
    if (digitalRead(PIR_PIN) == HIGH) {
      analogWrite(LED_PIN, 255);  // Restore brightness if motion is detected
      motionTimestamp = millis(); // Reset timer
      return;
    }
  }
  analogWrite(LED_PIN, 0);  // Turn off LED after dimming
}
