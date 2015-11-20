#include <SoftwareSerial.h>

void setup() {
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(8, INPUT);// For the click button
  Serial.begin(9600);
}

void BlueLight(int i){
  if(i == 0){
    digitalWrite(6, LOW);
    Serial.println("Blue Light turned off.");
  }else if(i == 1){
    digitalWrite(6, HIGH);
    Serial.println("Blue Light turned on.");
  }
}

void GreenLight(int i){
  if(i == 0){
    digitalWrite(5, LOW);
    Serial.println("Green Light turned off.");
  }else if(i == 1){
    digitalWrite(5, HIGH);
    Serial.println("Green Light turned on.");
  }
}

void RedLight(int i){
  if(i == 0){
    digitalWrite(4, LOW);
    Serial.println("Red Light turned off.");
  }else if(i == 1){
    digitalWrite(4, HIGH);
    Serial.println("Red Light turned on.");
  }
}

boolean isClick(){
  int digitalInput = digitalRead(8);
  if(digitalInput == 0) return true;
  else return false;
}

void loop() {
  Serial.write("loop Running\n");
  boolean isPress = isClick();
  if(isPress) RedLight(1);
  else RedLight(0);
  delay(1000); 
}

