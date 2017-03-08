#include <EEPROM.h>
  int stepPin = 26;
  int dirPin = 28;
  int enPin = 24;
  int led = 13;
  boolean light = true;
  int i = 0;
  int speedos = 1;
  boolean rot1 = true;
  int way = 9600;
  int usk = 1600;
  float g = 0;
  String sms;
     
void setup() {
  // put your setup code here, to run once:
pinMode(stepPin, OUTPUT);
pinMode(dirPin, OUTPUT);
pinMode(enPin, OUTPUT);
pinMode(led, OUTPUT);
digitalWrite(enPin, LOW);
digitalWrite(stepPin, LOW);
digitalWrite(dirPin, rot1);
}
void Talk() {
  if (Serial.available() > 0) {   //если есть сообщение
    sms = Serial.readString();    //читаем его
    Serial.print("I get->");
    Serial.println(sms);
    delay(100);
  }
  if (sms.substring(0,1) == "0") {
    Serial.println("change way");
    rot1 = !rot1;
    digitalWrite(dirPin, rot1);
  }
    else {
      i = (int(sms[0])-48)*1000;
      i = (int(sms[1])-48)*100;
      i = (int(sms[1])-48)*10;
      i = int(sms[3])-48;
      Serial.print("I get rotate ->");
      Serial.println(i);
      
      for (i = 0;i <=way; i++) {
        if (i < usk || (way - usk) < i){     
          g = abs(i-0.5*way)/100;
          int(g);
        }
        else { g = 0;}
          digitalWrite(stepPin, HIGH);
          delay(speedos+g);
          digitalWrite(stepPin, LOW);
          light = !light;
          digitalWrite(led, light);
          delay(speedos+g);
      }
    }
}

void loop() {
  // put your main code here, to run repeatedly:
  Talk();   
  delay(2000);
}
