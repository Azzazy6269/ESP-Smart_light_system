#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define FLASH_LED 14

// Wi-Fi Credentials
const char* ssid = "realme 6";
const char* password = "55555555";

// Firebase Credentials
#define FIREBASE_HOST "first-lighting-trial-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyCvrmdDmuCMjoBqtvSn7aYE8aSP3WBe3bA"

// Firebase and Wi-Fi objects
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
    Serial.begin(9600);
    pinMode(FLASH_LED, OUTPUT);

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
}

void loop() {
    // Read LED state from Firebase
    if (Firebase.RTDB.getInt(&firebaseData, "/room1/led")) {
        int ledState = firebaseData.intData();
        Serial.print("LED State: ");
        Serial.println(ledState);

        digitalWrite(FLASH_LED, ledState);
    } else {
        Serial.println("Failed to get data from Firebase");
    }

    delay(1000); // Check Firebase every second
}