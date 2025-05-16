#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define WIFI_SSID "RED ADSL"
#define WIFI_PASSWORD "RED01092641615"
#define FIREBASE_HOST "https://first-7cdc4-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "S9Wucv4MDnj4hwUogRE9ZaB0auwE26ZN37JwWccE"

FirebaseData firebaseData;
WiFiClient client;
String receivedData = "";

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (Serial.available()) {
    receivedData = Serial.readStringUntil('\n'); // Read data from Arduino Mega
    receivedData.trim();
    Serial.print("Received: ");
    Serial.println(receivedData);
    
    if (Firebase.setInt(firebaseData, "/sensor/value", receivedData.toInt())) {
      Serial.println("Data sent to Firebase");
    } else {
      Serial.print("Firebase Error: ");
      Serial.println(firebaseData.errorReason());
    }
  }
  delay(2000);
}

