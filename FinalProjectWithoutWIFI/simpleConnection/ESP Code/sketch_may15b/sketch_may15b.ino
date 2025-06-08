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


void setup() {
  Serial.begin(9600);

  pinMode(IR1_r1, INPUT);
  pinMode(IR2_r1, INPUT);
  pinMode(LED_r1, OUTPUT);

  pinMode(PIR_PIN_r1, INPUT);
 // ledcSetup(1, 5000, 8); // bug
 // ledcAttachPin(LED_PIR_r1, 1);

  ledcSetup(0, 5000, 8);       // channel 0, ferq 5 kh , resolution 8 bit (0-255)
  ledcAttachPin(LED_LDR, 0);   // connect channel to pin

  pinMode(IR1_r2, INPUT);
  pinMode(IR2_r2, INPUT);
  pinMode(LED_r2, OUTPUT);

  pinMode(PIR_PIN_r2, INPUT);
 // ledcSetup(2, 5000, 8); // bug
 // ledcAttachPin(LED_PIR_r2, 2);


  // No need to write pinMode(LDR_PIN, INPUT) as analog input pins are default to INPUT mode when you call analogRead()

}

void loop() {
 LDR_Check();
 Infrared_Check_r1();
 Infrared_Check_r2();

}

void LDR_Check(){
  ldrValue = analogRead(LDR_PIN);  // range 0:4095
  if (ldrValue > 3200) {  
    brightness = 0;      
  } else if (ldrValue > 1600) {  
    brightness = 128;    
  } else {  
    brightness = 255;    
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
   }else{
      digitalWrite(LED_r1, LOW);
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
   }else{
      digitalWrite(LED_r2, LOW);
   }
    
}

bool PIR_Check(int pin){
  if (digitalRead(pin) == HIGH) {
    return true; 
  } else {
    return false;
  }
}

