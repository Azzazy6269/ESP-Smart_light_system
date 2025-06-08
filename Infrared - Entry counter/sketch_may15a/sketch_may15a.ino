#define IR1 32
#define IR2 33
#define LED 14

int peopleCount = 0;
unsigned long t1 = 0, t2 = 0;
unsigned long lastTrigger1 = 0;
unsigned long lastTrigger2 = 0;
const unsigned long debounceDelay = 3000;

bool detected1 = false;
bool detected2 = false;

void setup() {
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
}

void loop() {
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
  }

  // LED logic
  if (peopleCount == 0) {
    digitalWrite(LED, LOW);
  } else {
    digitalWrite(LED, HIGH);
  }

  delay(50);
}

