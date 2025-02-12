#include <NewPing.h>


#define TR1 12
#define ECHO1 13
#define TR2 9
#define ECHO2 10
#define LED 11

NewPing us1(TR1, ECHO1, 200);
NewPing us2(TR2, ECHO2, 200);
int peopleCount =0;

void setup() {
  pinMode(TR1,OUTPUT);
  pinMode(ECHO1,INPUT);
  pinMode(TR2,OUTPUT);
  pinMode(ECHO2,INPUT);
  pinMode(LED,OUTPUT);

}

void loop() {
int dist1 = us1.ping_cm();
int dist2 = us2.ping_cm();

if (dist2<50){
  delay(100);
  if (us1.ping_cm()<50){
     peopleCount++;
  }
  delay(50);//stability
}
if (dist1<50){
  if(peopleCount>0){
  delay(100);
  if (us2.ping_cm()<50){
     peopleCount--;
  }
}
  delay(50);//stability
}
if (peopleCount==0){
   digitalWrite(LED,LOW);
}else{
   digitalWrite(LED,HIGH);
}
//delay(200);//stability
}

