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
#define IR1_r1 18
#define IR2_r1 19
#define PIR_PIN_r1 25 //27         

#define IR1_r2 32
#define IR2_r2 33
#define PIR_PIN_r2 35  

#define LED_LDR 12   
#define LED_r2 14
#define LED_r1 27
#define LED_r3 26


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
    pinMode(IR1_r1, INPUT);
    pinMode(IR2_r1, INPUT);
    pinMode(LED_r1, OUTPUT);

    pinMode(PIR_PIN_r1, INPUT);

   ledcSetup(0, 5000, 8);       // channel 0, ferq 5 kh , resolution 8 bit (0-255)
   ledcAttachPin(LED_LDR, 0);   // connect channel to pin

   pinMode(IR1_r2, INPUT);
   pinMode(IR2_r2, INPUT);
   pinMode(LED_r2, OUTPUT);
   pinMode(LED_r3, OUTPUT);

   pinMode(PIR_PIN_r2, INPUT);
  
 
}

void loop()
{
  for(int x=0; x<255; x++)
  {
    analogWrite(12,x);
    analogWrite(14,x);
    analogWrite(26,x);
    analogWrite(27,x);
    delay(15);
  }
  for(int x=255; x>0; x--)
  {
    analogWrite(12,x);
    analogWrite(14,x);
    analogWrite(26,x);
    analogWrite(27,x);
    delay(15);
  }
}