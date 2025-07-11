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

// Firebase and Wi-Fi objects
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

/***************************************************/
#define LDR_PIN 36   
#define LED_LDR 12   
#define IR1_r1 18
#define IR2_r1 19
#define PIR_PIN_r1 27          
#define LED_r1 2

#define IR1_r2 32
#define IR2_r2 33
#define PIR_PIN_r2 35          
#define LED_r2 14

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
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 3000;
/*******************************************************/

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

    /******************************************************/
    pinMode(IR1_r1, INPUT);
    pinMode(IR2_r1, INPUT);
    pinMode(LED_r1, OUTPUT);

    pinMode(PIR_PIN_r1, INPUT);

   ledcSetup(0, 5000, 8);       // channel 0, ferq 5 kh , resolution 8 bit (0-255)
   ledcAttachPin(LED_LDR, 0);   // connect channel to pin

   pinMode(IR1_r2, INPUT);
   pinMode(IR2_r2, INPUT);
   pinMode(LED_r2, OUTPUT);

   pinMode(PIR_PIN_r2, INPUT);
  /*********************************************************/
  ReadFromFirebase();

  //if (Firebase.RTDB.getBool(&firebaseData, "/Mode/Automatic")) {
  //  Automatic = firebaseData.boolData();
 
}

void loop() {
  if(Automatic){
    LDR_Check();
    Infrared_Check_r1();
    Infrared_Check_r2();
    if (millis() - lastSendTime > sendInterval) {
      SendToFirebase();
      lastSendTime = millis();
    }

  }else{
    if (millis() - lastSendTime > sendInterval) {
      ReadFromFirebase();
      lastSendTime = millis();
  }
    
}
}



void LDR_Check(){
  ldrValue = analogRead(LDR_PIN);  // range 0:4095
  if (ldrValue > 3200) {  
    brightness = 0; 
    currentLdr = 0;
  } else if (ldrValue > 1600) {  
    brightness = 128;
    currentLdr = 1;    
  } else {  
    brightness = 255;
    currentLdr = 1;    
  }ledcWrite(0, brightness);  // channel 0
}

void Infrared_Check_r1(){
  unsigned long currentTime = millis();

  int ir1State = digitalRead(IR1_r1);
  int ir2State = digitalRead(IR2_r1);

  // IR1
  if (ir1State == LOW && !detected1_r1 && currentTime - lastTrigger1_r1 > debounceDelay) {
    t1_r1 = currentTime;
    detected1_r1 = true;
    lastTrigger1_r1 = currentTime;
    Serial.println("IR1 triggered");
  }

  // IR2
  if (ir2State == LOW && !detected2_r1 && currentTime - lastTrigger2_r1 > debounceDelay) {
    t2_r1 = currentTime;
    detected2_r1 = true;
    lastTrigger2_r1 = currentTime;
    Serial.println("IR2 triggered");
  }

  // Check both sensors triggered
  if (detected1_r1 && detected2_r1) {
    if (abs((long)t1_r1 - (long)t2_r1) < 1500) { // ensure it’s within expected time
      if (t1_r1 < t2_r1) {
        peopleCount_r1++;
        Serial.println("Person Entered");
      } else {
        if (peopleCount_r1 > 0) {
          peopleCount_r1--;
          Serial.println("Person Exited");
        }
      }
    }

    detected1_r1 = false;
    detected2_r1 = false;
    t1_r1 = 0;
    t2_r1 = 0;
  }else if(detected1_r1 && millis()-t1_r1 > 1500){
     detected1_r1 = false;
    detected2_r1 = false;
    t1_r1 = 0;
    t2_r1 = 0;
  }
  else if(detected2_r1 && millis()-t2_r1 > 1500){
     detected1_r1 = false;
    detected2_r1 = false;
    t1_r1 = 0;
    t2_r1 = 0;
  }
   if(PIR_Check(PIR_PIN_r1) && peopleCount_r1 > 0){
      digitalWrite(LED_r1, HIGH);
      currentLed_r1 = 1;
   }else{
      digitalWrite(LED_r1, LOW);
      currentLed_r1 = 0;
   }
    
}

 void Infrared_Check_r2(){
  unsigned long currentTime = millis();

  int ir1State = digitalRead(IR1_r2);
  int ir2State = digitalRead(IR2_r2);

  // IR1
  if (ir1State == LOW && !detected1_r2 && currentTime - lastTrigger1_r2 > debounceDelay) {
    t1_r2 = currentTime;
    detected1_r2 = true;
    lastTrigger1_r2 = currentTime;
    Serial.println("IR1 triggered");
  }

  // IR2
  if (ir2State == LOW && !detected2_r2 && currentTime - lastTrigger2_r2 > debounceDelay) {
    t2_r2 = currentTime;
    detected2_r2 = true;
    lastTrigger2_r2 = currentTime;
    Serial.println("IR2 triggered");
  }

  // Check both sensors triggered
  if (detected1_r2 && detected2_r2) {
    if (abs((long)t1_r2 - (long)t2_r2) < 3000) { // ensure it’s within expected time
      if (t1_r2 < t2_r2) {
        peopleCount_r2++;
        Serial.println("Person Entered");
      } else {
        if (peopleCount_r2 > 0) {
          peopleCount_r2--;
          Serial.println("Person Exited");
        }
      }
    }

    detected1_r2 = false;
    detected2_r2 = false;
    t1_r2 = 0;
    t2_r2 = 0;
  }else if(detected1_r2 && millis()-t1_r2 > 1500){
     detected1_r2 = false;
    detected2_r2 = false;
    t1_r2 = 0;
    t2_r2 = 0;
  }
  else if(detected2_r2 && millis()-t2_r2 > 1500){
     detected1_r2 = false;
    detected2_r2 = false;
    t1_r2 = 0;
    t2_r2 = 0;
  }

   if(PIR_Check(PIR_PIN_r2) && peopleCount_r2 > 0){
      digitalWrite(LED_r2, HIGH);
      currentLed_r2 = 1;
   }else{
      digitalWrite(LED_r2, LOW);
      currentLed_r2 = 0;
   }
    
}

bool PIR_Check(int pin){
  if (digitalRead(pin) == HIGH) {
    return true; 
  } else {
    return false;
  }
}

void SendToFirebase(){
  if(lastLed_r1 != currentLed_r1){
    if(Firebase.RTDB.setInt(&firebaseData, "/room1/led",digitalRead(LED_r1))) {
      lastLed_r1 = currentLed_r1;
    }
  }
  if(lastLed_r2 != currentLed_r2){
    if(Firebase.RTDB.setInt(&firebaseData, "/room2/led",digitalRead(LED_r2) )){
      lastLed_r2 = currentLed_r2;
    }
  }
  if(lastLdr != currentLdr){
    if(Firebase.RTDB.setInt(&firebaseData, "/outing/brightness",brightness )){
      lastLdr = currentLdr;
    }
  }
  if (Firebase.RTDB.getBool(&firebaseData, "/Mode/Automatic")) {
    Automatic = firebaseData.boolData(); 
 }
}
void ReadFromFirebase(){
  if (Firebase.RTDB.getInt(&firebaseData, "/room1/led")) {
     currentLed_r1 = firebaseData.intData();
     digitalWrite(LED_r1, currentLed_r1);
     
  }
  if (Firebase.RTDB.getInt(&firebaseData, "/room2/led")) {
     currentLed_r2 = firebaseData.intData();
     digitalWrite(LED_r2, currentLed_r2);
  }
  if (Firebase.RTDB.getInt(&firebaseData, "/outing/brightness")) {
     currentLdr = firebaseData.intData();
     ledcWrite(0, currentLdr);
  }
  if (Firebase.RTDB.getBool(&firebaseData, "/Mode/Automatic")) {
    Automatic = firebaseData.boolData();
    if(Automatic){
      lastLed_r1 = !currentLed_r1;
      lastLed_r2 = !currentLed_r2;
      lastLdr = !currentLdr;
      SendToFirebase();
    }
 }
}

