#include <SoftwareSerial.h>

SoftwareSerial XBee(2, 3);

void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int result = 1;
  XBee.println("hahahaha");
  Serial.println("send======");
  delay(100);
}
