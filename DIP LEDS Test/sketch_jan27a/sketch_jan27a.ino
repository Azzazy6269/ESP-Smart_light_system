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

void loop(){
  //ledcWrite(0, 255);
  digitalWrite(LED_r1, HIGH);
  digitalWrite(LED_r2, HIGH);
  digitalWrite(LED_r3, HIGH);
  delay(2000);
  //ledcWrite(0, 0);  
  digitalWrite(LED_r1, LOW);
  digitalWrite(LED_r2, LOW);
  digitalWrite(LED_r3, LOW);
  delay(2000);
}