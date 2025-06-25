#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Wi-Fi Credentials
const char* ssid = "realme 6";
const char* password = "55555555";

// Firebase Credentials
#define FIREBASE_HOST "first-lighting-trial-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyCvrmdDmuCMjoBqtvSn7aYE8aSP3WBe3bA"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Pins
#define LDR_PIN 36
#define LED_LDR 12

#define IR1_r1 18
#define IR2_r1 19
#define PIR_r1 27
#define LED_r1 2

#define IR1_r2 32
#define IR2_r2 33
#define PIR_r2 35
#define LED_r2 14

// Shared variables
volatile int brightness = 0;
volatile bool autoMode_r1 = true, autoMode_r2 = true;
volatile int ledState_r1 = 0, ledState_r2 = 0;

volatile int peopleCount_r1 = 0;
volatile int peopleCount_r2 = 0;

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 3000;

void setup() {
  Serial.begin(9600);

  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nWiFi Connected");

  // Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // PWM for LDR LED
  ledcSetup(0, 5000, 8);
  ledcAttachPin(LED_LDR, 0);

  // Pins
  pinMode(IR1_r1, INPUT); pinMode(IR2_r1, INPUT); pinMode(PIR_r1, INPUT); pinMode(LED_r1, OUTPUT);
  pinMode(IR1_r2, INPUT); pinMode(IR2_r2, INPUT); pinMode(PIR_r2, INPUT); pinMode(LED_r2, OUTPUT);

  // Get initial mode from Firebase
  Firebase.RTDB.getBool(&fbdo, "/Mode/Automatic_r1");
  autoMode_r1 = fbdo.boolData();
  Firebase.RTDB.getBool(&fbdo, "/Mode/Automatic_r2");
  autoMode_r2 = fbdo.boolData();

  // Tasks
  xTaskCreatePinnedToCore(TaskRoom1, "Room1Task", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskRoom2, "Room2Task", 4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskLDR, "LDRTask", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskFirebase, "FirebaseTask", 8192, NULL, 1, NULL, 0);
}

// Room 1 Logic
void TaskRoom1(void *pvParameters) {
  bool ir1Detected = false, ir2Detected = false;
  unsigned long t1 = 0, t2 = 0;

  for (;;) {
    if (autoMode_r1) {
      int ir1 = digitalRead(IR1_r1);
      int ir2 = digitalRead(IR2_r1);

      if (ir1 == LOW && !ir1Detected) { ir1Detected = true; t1 = millis(); }
      if (ir2 == LOW && !ir2Detected) { ir2Detected = true; t2 = millis(); }

      if (ir1Detected && ir2Detected) {
        if (abs((long)t1 - (long)t2) < 2000) {
          if (t1 < t2) peopleCount_r1++;
          else if (peopleCount_r1 > 0) peopleCount_r1--;
        }
        ir1Detected = ir2Detected = false;
      }

      if (digitalRead(PIR_r1) == HIGH && peopleCount_r1 > 0) {
        digitalWrite(LED_r1, HIGH); ledState_r1 = 1;
      } else {
        digitalWrite(LED_r1, LOW); ledState_r1 = 0;
      }
    }

    delay(100);
  }
}

// Room 2 Logic
void TaskRoom2(void *pvParameters) {
  bool ir1Detected = false, ir2Detected = false;
  unsigned long t1 = 0, t2 = 0;

  for (;;) {
    if (autoMode_r2) {
      int ir1 = digitalRead(IR1_r2);
      int ir2 = digitalRead(IR2_r2);

      if (ir1 == LOW && !ir1Detected) { ir1Detected = true; t1 = millis(); }
      if (ir2 == LOW && !ir2Detected) { ir2Detected = true; t2 = millis(); }

      if (ir1Detected && ir2Detected) {
        if (abs((long)t1 - (long)t2) < 2000) {
          if (t1 < t2) peopleCount_r2++;
          else if (peopleCount_r2 > 0) peopleCount_r2--;
        }
        ir1Detected = ir2Detected = false;
      }

      if (digitalRead(PIR_r2) == HIGH && peopleCount_r2 > 0) {
        digitalWrite(LED_r2, HIGH); ledState_r2 = 1;
      } else {
        digitalWrite(LED_r2, LOW); ledState_r2 = 0;
      }
    }

    delay(100);
  }
}

// LDR Brightness Task
void TaskLDR(void *pvParameters) {
  for (;;) {
    int ldr = analogRead(LDR_PIN);
    if (ldr > 3200) brightness = 0;
    else if (ldr > 1600) brightness = 128;
    else brightness = 255;

    ledcWrite(0, brightness);

    delay(1000);
  }
}

// Firebase Task
void TaskFirebase(void *pvParameters) {
  for (;;) {
    if (Firebase.ready() && millis() - lastSendTime > sendInterval) {
      lastSendTime = millis();

      // Send data
      Firebase.RTDB.setInt(&fbdo, "/rooms/room_one", ledState_r1);
      Firebase.RTDB.setInt(&fbdo, "/rooms/room_two", ledState_r2);
      Firebase.RTDB.setInt(&fbdo, "/outing/brightness", brightness);

      // Read Modes
      if (Firebase.RTDB.getBool(&fbdo, "/Mode/Automatic_r1"))
        autoMode_r1 = fbdo.boolData();
      if (Firebase.RTDB.getBool(&fbdo, "/Mode/Automatic_r2"))
        autoMode_r2 = fbdo.boolData();

      // Manual control (if auto is false)
      if (!autoMode_r1 && Firebase.RTDB.getInt(&fbdo, "/rooms/room_one"))
        digitalWrite(LED_r1, fbdo.intData());

      if (!autoMode_r2 && Firebase.RTDB.getInt(&fbdo, "/rooms/room_two"))
        digitalWrite(LED_r2, fbdo.intData());

      if (!autoMode_r1 && Firebase.RTDB.getInt(&fbdo, "/outing/brightness"))
        ledcWrite(0, fbdo.intData());
    }

    delay(500);
  }
}
void loop(){
  
}
