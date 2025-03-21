#include <NewPing.h>

#define TR1 9
#define ECHO1 10
#define TR2 6
#define ECHO2 7
#define LED 8

NewPing us1(TR1, ECHO1, 200);
NewPing us2(TR2, ECHO2, 200);

int peopleCount = 0;
unsigned long t1 = 0, t2 = 0; // Store timestamps

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600); // Debugging
}

void loop() {
  int dist1 = us1.ping_cm(); // Distance from sensor 1
  int dist2 = us2.ping_cm(); // Distance from sensor 2

  if (dist1 > 0 && dist1 < 50) {  // Someone detected at sensor 1
    t1 = millis();
  }
  if (dist2 > 0 && dist2 < 50) {  // Someone detected at sensor 2
    t2 = millis();
  }

  // Check the sequence of movement
  if (t1 > 0 && t2 > 0) {  // Both timestamps must be set
    if (t1 < t2) {  // Person entered
      peopleCount++;
      Serial.println("Person entered!");
    } else if (t2 < t1) {  // Person exited
      if (peopleCount > 0) {
        peopleCount--;
        Serial.println("Person exited!");
      }
    }
    // Reset timestamps after detecting movement
    t1 = 0;
    t2 = 0;
  }

  // LED control
  if (peopleCount == 0) {
    digitalWrite(LED, LOW);
  } else {
    digitalWrite(LED, HIGH);
  }


  //delay(50); // Short delay for stability
}
