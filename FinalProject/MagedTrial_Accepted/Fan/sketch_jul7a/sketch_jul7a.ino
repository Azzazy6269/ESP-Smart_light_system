#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <DHT11.h>

// Wi-Fi Credentials
const char* ssid = "Await Connection";
const char* password = "MGD.914614??bsbs.meow";

// Firebase Credentials
#define FIREBASE_HOST "graduation-project-bumgd17-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyDr7hgjahxcQkUsBAlB6MEi9Mjt14MZe5A"

// Firebase and Wi-Fi objects
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Pin definitions
#define LDR_PIN 36   
#define IR1_r1 19
#define IR2_r1 18
#define PIR_PIN_r1 35          
#define IR1_r2 32
#define IR2_r2 33
#define PIR_PIN_r2 5          
#define LED_r1 27
#define LED_r2 14
#define LED_r3 25
#define LED_LDR 26
#define motorPinA 22 // Fan control pin A (PWM-capable)
#define motorPinB 23 // Fan control pin B (PWM-capable)

int ldrValue;       
int brightness;

int peopleCount_r1 = 0;
unsigned long t1_r1 = 0, t2_r1 = 0;
unsigned long lastTrigger1_r1 = 0;
unsigned long lastTrigger2_r1 = 0;
bool detected1_r1 = false;
bool detected2_r1 = false;

int peopleCount_r2 = 0;
unsigned long t1_r2 = 0, t2_r2 = 0;
unsigned long lastTrigger1_r2 = 0;
unsigned long lastTrigger2_r2 = 0;
bool detected1_r2 = false;
bool detected2_r2 = false;

const unsigned long debounceDelay = 800;

int lastLed_r1 = 0;
int lastLed_r2 = 0;
int lastLdr = 0;
int currentLed_r1 = 0;
int currentLed_r2 = 0;
int currentLdr = 0;

bool Automatic = true;
unsigned long lastSend = 0;
unsigned long sendInterval = 1500;

DHT11 dht11(2);
int temperature = 0;
int humidity = 0;
int temperature_firebase = 30;
int humidity_firebase = 65;
int temperature_firebase_last = 0;
int humidity_firebase_last = 0;
int fan_current = 0; // 0: off, 1: clockwise full, 2: anticlockwise half, 3: clockwise 75%
int fan_last = 0;

// Fan control timing
unsigned long fanStartTime = 0;
const unsigned long fanInterval = 2000; // 2 seconds per state
const unsigned long stopInterval = 1000; // 1 second stop
int fanState = 0; // 0: clockwise full, 1: stop1, 2: anticlockwise half, 3: stop2, 4: clockwise 75%

void setup() {
  Serial.begin(9600);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  // Set up Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Initialize pins
  pinMode(IR1_r1, INPUT);
  pinMode(IR2_r1, INPUT);
  pinMode(LED_r1, OUTPUT);
  pinMode(PIR_PIN_r1, INPUT);
  pinMode(IR1_r2, INPUT);
  pinMode(IR2_r2, INPUT);
  pinMode(LED_r2, OUTPUT);
  pinMode(PIR_PIN_r2, INPUT);
  pinMode(motorPinA, OUTPUT);
  pinMode(motorPinB, OUTPUT);

  ledcSetup(0, 5000, 8); // Channel 0, 5 kHz, 8-bit resolution
  ledcAttachPin(LED_LDR, 0);

  ManualFirebase();
}

void loop() {
  if (Automatic) {
    sendInterval = 3000;
    LDR_Check();
    Infrared_Check_r1();
    Infrared_Check_r2();
    DHT11_Check();
    if (millis() - lastSend > sendInterval) {
      AutomaticFirebase();
      lastSend = millis();
    }
  } else {
    sendInterval = 200;
    DHT11_Check();
    if (millis() - lastSend > sendInterval) {
      ManualFirebase();
      lastSend = millis();
    }
  }
}

void controlFan(int state) {
  switch (state) {
    case 0: // Clockwise full speed
      digitalWrite(motorPinA, HIGH);
      digitalWrite(motorPinB, LOW);
      break;
    case 1: // Stop
    case 3:
      digitalWrite(motorPinA, LOW);
      digitalWrite(motorPinB, LOW);
      break;
    case 2: // Anticlockwise half speed
      analogWrite(motorPinA, 0);
      analogWrite(motorPinB, 128); // 50% speed
      break;
    case 4: // Clockwise 75% speed
      analogWrite(motorPinA, 192); // 75% speed
      analogWrite(motorPinB, 0);
      break;
  }
}

void LDR_Check() {
  ldrValue = analogRead(LDR_PIN);  
  if (ldrValue > 3200) {  
    brightness = 0; 
    currentLdr = 0;
  } else if (ldrValue > 1600) {  
    brightness = 128;
    currentLdr = 1;    
  } else {  
    brightness = 255;
    currentLdr = 1;    
  }
  ledcWrite(0, brightness);
}

void Infrared_Check_r1() {
  unsigned long currentTime = millis();
  int ir1State = digitalRead(IR1_r1);
  int ir2State = digitalRead(IR2_r1);

  if (ir1State == LOW && !detected1_r1 && currentTime - lastTrigger1_r1 > debounceDelay) {
    t1_r1 = currentTime;
    detected1_r1 = true;
    lastTrigger1_r1 = currentTime;
    Serial.println("IR1 triggered1");
  }

  if (ir2State == LOW && !detected2_r1 && currentTime - lastTrigger2_r1 > debounceDelay) {
    t2_r1 = currentTime;
    detected2_r1 = true;
    lastTrigger2_r1 = currentTime;
    Serial.println("IR2 triggered1");
  }

  if (detected1_r1 && detected2_r1) {
    if (abs((long)t1_r1 - (long)t2_r1) < 1500) {
      if (t1_r1 < t2_r1) {
        peopleCount_r1++;
        Serial.println("Person Entered1");
      } else if (peopleCount_r1 > 0) {
        peopleCount_r1--;
        Serial.println("Person Exited1");
      }
    }
    detected1_r1 = false;
    detected2_r1 = false;
    t1_r1 = 0;
    t2_r1 = 0;
  } else if (detected1_r1 && millis() - t1_r1 > 1500) {
    detected1_r1 = false;
    detected2_r1 = false;
    t1_r1 = 0;
    t2_r1 = 0;
  } else if (detected2_r1 && millis() - t2_r1 > 1500) {
    detected1_r1 = false;
    detected2_r1 = false;
    t1_r1 = 0;
    t2_r1 = 0;
  }

  if (PIR_Check(PIR_PIN_r1) && peopleCount_r1 > 0) {
    digitalWrite(LED_r1, HIGH);
    currentLed_r1 = 1;
  } else {
    digitalWrite(LED_r1, LOW);
    currentLed_r1 = 0;
  }
}

void Infrared_Check_r2() {
  unsigned long currentTime = millis();
  int ir1State = digitalRead(IR1_r2);
  int ir2State = digitalRead(IR2_r2);

  if (ir1State == LOW && !detected1_r2 && currentTime - lastTrigger1_r2 > debounceDelay) {
    t1_r2 = currentTime;
    detected1_r2 = true;
    lastTrigger1_r2 = currentTime;
    Serial.println("IR1 triggered2");
  }

  if (ir2State == LOW && !detected2_r2 && currentTime - lastTrigger2_r2 > debounceDelay) {
    t2_r2 = currentTime;
    detected2_r2 = true;
    lastTrigger2_r2 = currentTime;
    Serial.println("IR2 triggered2");
  }

  if (detected1_r2 && detected2_r2) {
    if (abs((long)t1_r2 - (long)t2_r2) < 3000) {
      if (t1_r2 < t2_r2) {
        peopleCount_r2++;
        Serial.println("Person Entered2");
      } else if (peopleCount_r2 > 0) {
        peopleCount_r2--;
        Serial.println("Person Exited2");
      }
    }
    detected1_r2 = false;
    detected2_r2 = false;
    t1_r2 = 0;
    t2_r2 = 0;
  } else if (detected1_r2 && millis() - t1_r2 > 1500) {
    detected1_r2 = false;
    detected2_r2 = false;
    t1_r2 = 0;
    t2_r2 = 0;
  } else if (detected2_r2 && millis() - t2_r2 > 1500) {
    detected1_r2 = false;
    detected2_r2 = false;
    t1_r2 = 0;
    t2_r2 = 0;
  }

  if (PIR_Check(PIR_PIN_r2) && peopleCount_r2 > 0) {
    digitalWrite(LED_r2, HIGH);
    currentLed_r2 = 1;
  } else {
    digitalWrite(LED_r2, LOW);
    currentLed_r2 = 0;
  }
}

bool PIR_Check(int pin) {
  return digitalRead(pin) == HIGH;
}

void DHT11_Check() {
  int result = dht11.readTemperatureHumidity(temperature, humidity);
  if (result == 0) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    temperature_firebase = temperature;
    Serial.print(" °C\tHumidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    humidity_firebase = humidity;
  } else {
    Serial.println(DHT11::getErrorString(result));
  }

  if (Automatic) {
    if (temperature_firebase > 25) {
      unsigned long currentTime = millis();
      if (fanState == 0 && currentTime - fanStartTime >= fanInterval) {
        fanState = 1;
        fanStartTime = currentTime;
      } else if (fanState == 1 && currentTime - fanStartTime >= stopInterval) {
        fanState = 2;
        fanStartTime = currentTime;
      } else if (fanState == 2 && currentTime - fanStartTime >= fanInterval) {
        fanState = 3;
        fanStartTime = currentTime;
      } else if (fanState == 3 && currentTime - fanStartTime >= stopInterval) {
        fanState = 4;
        fanStartTime = currentTime;
      } else if (fanState == 4 && currentTime - fanStartTime >= fanInterval) {
        fanState = 0;
        fanStartTime = currentTime;
      }
      fan_current = fanState;
      controlFan(fanState);
    } else {
      fanState = 1; // Stop
      fan_current = 1;
      controlFan(fanState);
      fanStartTime = millis();
    }
  }
}

void AutomaticFirebase() {
  if (lastLed_r1 != currentLed_r1) {
    if (Firebase.RTDB.setInt(&firebaseData, "/rooms/room_one", digitalRead(LED_r1))) {
      lastLed_r1 = currentLed_r1;
    }
  }
  if (lastLed_r2 != currentLed_r2) {
    if (Firebase.RTDB.setInt(&firebaseData, "/rooms/room_two", digitalRead(LED_r2))) {
      lastLed_r2 = currentLed_r2;
    }
  }
  if (lastLdr != currentLdr) {
    if (Firebase.RTDB.setInt(&firebaseData, "/outing/brightness", brightness)) {
      lastLdr = currentLdr;
    }
  }
  if (temperature_firebase_last != temperature_firebase) {
    if (Firebase.RTDB.setInt(&firebaseData, "/rooms/temp_sensor", temperature_firebase)) {
      temperature_firebase_last = temperature_firebase;
    }
  }
  if (humidity_firebase_last != humidity_firebase) {
    if (Firebase.RTDB.setInt(&firebaseData, "/rooms/humidity", humidity_firebase)) {
      humidity_firebase_last = humidity_firebase;
    }
  }
  if (fan_last != fan_current) {
    if (Firebase.RTDB.setInt(&firebaseData, "/rooms/fan", fan_current)) {
      fan_last = fan_current;
    }
  }
  if (Firebase.RTDB.getBool(&firebaseData, "/Mode/Automatic")) {
    Automatic = firebaseData.boolData();
  }
}

void ManualFirebase() {
  if (Firebase.RTDB.getInt(&firebaseData, "/rooms/room_one")) {
    currentLed_r1 = firebaseData.intData();
    digitalWrite(LED_r1, currentLed_r1);
  }
  if (Firebase.RTDB.getInt(&firebaseData, "/rooms/room_two")) {
    currentLed_r2 = firebaseData.intData();
    digitalWrite(LED_r2, currentLed_r2);
  }
  if (Firebase.RTDB.getInt(&firebaseData, "/outing/brightness")) {
    currentLdr = firebaseData.intData();
    ledcWrite(0, currentLdr);
  }
  if (Firebase.RTDB.getInt(&firebaseData, "/rooms/fan")) {
    fan_current = firebaseData.intData();
    controlFan(fan_current);
  }
  if (temperature_firebase_last != temperature_firebase) {
    if (Firebase.RTDB.setInt(&firebaseData, "/rooms/temp_sensor", temperature_firebase)) {
      temperature_firebase_last = temperature_firebase;
    }
  }
  if (humidity_firebase_last != humidity_firebase) {
    if (Firebase.RTDB.setInt(&firebaseData, "/rooms/humidity", humidity_firebase)) {
      humidity_firebase_last = humidity_firebase;
    }
  }
  if (fan_last != fan_current) {
    if (Firebase.RTDB.setInt(&firebaseData, "/rooms/fan", fan_current)) {
      fan_last = fan_current;
    }
  }
  if (Firebase.RTDB.getBool(&firebaseData, "/Mode/Automatic")) {
    Automatic = firebaseData.boolData();
    if (Automatic) {
      lastLed_r1 = !currentLed_r1;
      lastLed_r2 = !currentLed_r2;
      lastLdr = !currentLdr;
      fan_last = !fan_current;
      AutomaticFirebase();
    }
  }
}