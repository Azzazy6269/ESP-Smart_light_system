#define LDR_PIN A0   // LDR sensor connected to analog pin A0
#define LED_PIN 9    // LED connected to PWM pin 9

int ldrValue;        // Variable to store LDR reading
int brightness;      // Variable for LED brightness

void setup() {
    pinMode(LED_PIN, OUTPUT);
    // No need to write pinMode(LDR_PIN, INPUT) as analog input pins are default to INPUT mode when you call analogRead()
}

void loop() {
    ldrValue = analogRead(LDR_PIN);  // Read LDR sensor value (0-1023)
  

    if (ldrValue > 800) {  // Bright sunlight
        brightness = 0;    // Turn LED off (daytime)
    } else if (ldrValue > 400) {  // Evening
        brightness = 128;  // Medium brightness
    } else {               // Night
        brightness = 255;  // Full brightness
    }
   // brightness = map(ldrValue, 0, 1023, 255, 0);
    analogWrite(LED_PIN, brightness);  // Control LED brightness
}
