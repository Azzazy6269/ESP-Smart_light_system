#define LDR_PIN 36   
#define LED_LDR 12   
#define IR1 18
#define IR2 19
#define LED_IR 2
#define PIR_PIN 27          
#define LED_PIR 14

int ldrValue;       
int brightness;
int peopleCount = 0;
unsigned long t1 = 0, t2 = 0;
unsigned long lastTrigger1 = 0;
unsigned long lastTrigger2 = 0;
const unsigned long debounceDelay = 1500;

bool detected1 = false;
bool detected2 = false;


void setup() {
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(LED_IR, OUTPUT);
  Serial.begin(9600);

  pinMode(PIR_PIN, INPUT);
  ledcSetup(1, 5000, 8);
  ledcAttachPin(LED_PIR, 1);

  ledcSetup(0, 5000, 8);       // channel 0, ferq 5 kh , resolution 8 bit (0-255)
  ledcAttachPin(LED_LDR, 0);   // connect channel to pin

  // No need to write pinMode(LDR_PIN, INPUT) as analog input pins are default to INPUT mode when you call analogRead()

}

void loop() {
 LDR_Check();
 Infrared_Check();

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

void Infrared_Check(){
  unsigned long currentTime = millis();

  int ir1State = digitalRead(IR1);
  int ir2State = digitalRead(IR2);

  // IR1
  if (ir1State == LOW && !detected1 && currentTime - lastTrigger1 > debounceDelay) {
    t1 = currentTime;
    detected1 = true;
    lastTrigger1 = currentTime;
    Serial.println("IR1 triggered");
  }

  // IR2
  if (ir2State == LOW && !detected2 && currentTime - lastTrigger2 > debounceDelay) {
    t2 = currentTime;
    detected2 = true;
    lastTrigger2 = currentTime;
    Serial.println("IR2 triggered");
  }

  // Check both sensors triggered
  if (detected1 && detected2) {
    if (abs((long)t1 - (long)t2) < 3000) { // ensure it’s within expected time
      if (t1 < t2) {
        peopleCount++;
        Serial.println("Person Entered");
      } else {
        if (peopleCount > 0) {
          peopleCount--;
          Serial.println("Person Exited");
        }
      }
    }

    detected1 = false;
    detected2 = false;
    t1 = 0;
    t2 = 0;
  }else if(detected1 && millis()-t1 > 1500){
     detected1 = false;
    detected2 = false;
    t1 = 0;
    t2 = 0;
  }
  else if(detected2 && millis()-t2 > 1500){
     detected1 = false;
    detected2 = false;
    t1 = 0;
    t2 = 0;
  }

  // LED logic
   if(PIR_Check() && peopleCount > 0){
      digitalWrite(LED_IR, HIGH);
   }else{
      digitalWrite(LED_IR, LOW);
   }
    
  }

 

bool PIR_Check(){
  if (digitalRead(PIR_PIN) == HIGH) {
    return true;
   // ledcWrite(1, 255);  
  } else {
    return false;
    //ledcWrite(1, 0);    
   // delay(5000);
  }
}
