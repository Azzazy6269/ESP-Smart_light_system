#include "DHT.h"

#define DHTPIN 4         // متصل بالرجل رقم 2 في الأردوينو
#define DHTTYPE DHT22    // نوع الحساس

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float temp = dht.readTemperature();     // القراءة بالدرجة المئوية
  float humi = dht.readHumidity();

  // التحقق من صحة القراءة
  if (isnan(temp) || isnan(humi)) {
    Serial.println("فشل في قراءة الحساس!");
    
  }

  Serial.print("درجة الحرارة: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("الرطوبة: ");
  Serial.print(humi);
  Serial.println(" %");

  delay(2000);  // يجب الانتظار على الأقل 2 ثانية بين كل قراءة
}
