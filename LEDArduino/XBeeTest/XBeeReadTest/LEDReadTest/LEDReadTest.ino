#include <SoftwareSerial.h>

SoftwareSerial XBee(2, 3); 

void setup() {
  XBee.begin(9600);
  Serial.begin(9600);
}

void loop() {
  Serial.write("loop Running\n");
  char stat = XBee.read();
  Serial.println(stat);
  stat = XBee.read();
  Serial.println(stat);
  Serial.println("===end===");
  delay(2000);
}


